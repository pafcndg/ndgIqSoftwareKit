//
//  ThunderCircularProgressViewLayer.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 09/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import Foundation

class ThunderCircularProgressViewLayer: CALayer {

    @NSManaged var angle: Int
    var radius: CGFloat! {
        didSet {
            invalidateGradientCache()
        }
    }
    var startAngle: Int!
    var clockwise: Bool! {
        didSet {
            if clockwise != oldValue {
                invalidateGradientCache()
            }
        }
    }
    var roundedCorners: Bool!
    var gradientRotateSpeed: CGFloat! {
        didSet {
            invalidateGradientCache()
        }
    }
    var glowAmount: CGFloat!
    var glowMode: ThunderCircularProgressGlowMode!
    var progressThickness: CGFloat!
    var trackThickness: CGFloat!
    var trackColor: UIColor!
    var progressInsideFillColor: UIColor = UIColor.clearColor()
    var colorsArray: [UIColor]! {
        didSet {
            invalidateGradientCache()
        }
    }
    private var gradientCache: CGGradientRef?
    private var locationsCache: [CGFloat]?
    
    private struct GlowConstants {
        private static let sizeToGlowRatio: CGFloat = 0.00015
        static func glowAmountForAngle(angle: Int, glowAmount: CGFloat, glowMode: ThunderCircularProgressGlowMode, size: CGFloat) -> CGFloat {
            switch glowMode {
            case .Forward:
                return CGFloat(angle) * size * sizeToGlowRatio * glowAmount
            case .Reverse:
                return CGFloat(360 - angle) * size * sizeToGlowRatio * glowAmount
            case .Constant:
                return 360 * size * sizeToGlowRatio * glowAmount
            default:
                return 0
            }
        }
    }
    
    override class func needsDisplayForKey(key: String) -> Bool {
        return key == "angle" ? true : super.needsDisplayForKey(key)
    }
    
    override init(layer: AnyObject) {
        super.init(layer: layer)
        let progressLayer = layer as! ThunderCircularProgressViewLayer
        radius = progressLayer.radius
        angle = progressLayer.angle
        startAngle = progressLayer.startAngle
        clockwise = progressLayer.clockwise
        roundedCorners = progressLayer.roundedCorners
        gradientRotateSpeed = progressLayer.gradientRotateSpeed
        glowAmount = progressLayer.glowAmount
        glowMode = progressLayer.glowMode
        progressThickness = progressLayer.progressThickness
        trackThickness = progressLayer.trackThickness
        trackColor = progressLayer.trackColor
        colorsArray = progressLayer.colorsArray
    }
    
    override init() {
        super.init()
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
    }
    
    override func drawInContext(ctx: CGContext) {
        UIGraphicsPushContext(ctx)
        let rect = bounds
        let size = rect.size
        
        let trackLineWidth: CGFloat = radius * trackThickness
        let progressLineWidth = radius * progressThickness
        let arcRadius = max(radius - trackLineWidth/2, radius - progressLineWidth/2)
        CGContextAddArc(ctx, CGFloat(size.width/2.0), CGFloat(size.height/2.0), arcRadius, 0, CGFloat(M_PI * 2), 0)
        trackColor.set()
        CGContextSetStrokeColorWithColor(ctx, trackColor.CGColor)
        CGContextSetFillColorWithColor(ctx, progressInsideFillColor.CGColor)
        CGContextSetLineWidth(ctx, trackLineWidth)
        CGContextSetLineCap(ctx, CGLineCap.Butt)
        CGContextDrawPath(ctx, .FillStroke)
        
        UIGraphicsBeginImageContextWithOptions(size, false, 0.0)
        let imageCtx = UIGraphicsGetCurrentContext()
        let reducedAngle = ThunderCircularProgressUtils.Mod(angle, range: 360, minMax: (0, 360))
        let fromAngle = ThunderCircularProgressUtils.DegreesToRadians(CGFloat(-startAngle))
        let toAngle = ThunderCircularProgressUtils.DegreesToRadians(CGFloat((clockwise == true ? -reducedAngle : reducedAngle) - startAngle))
        CGContextAddArc(imageCtx, CGFloat(size.width/2.0),CGFloat(size.height/2.0), arcRadius, fromAngle, toAngle, clockwise == true ? 1 : 0)
        let glowValue = GlowConstants.glowAmountForAngle(reducedAngle, glowAmount: glowAmount, glowMode: glowMode, size: size.width)
        if glowValue > 0 {
            CGContextSetShadowWithColor(imageCtx, CGSizeZero, glowValue, UIColor.blackColor().CGColor)
        }
        CGContextSetLineCap(imageCtx, roundedCorners == true ? .Round : .Butt)
        CGContextSetLineWidth(imageCtx, progressLineWidth)
        CGContextDrawPath(imageCtx, .Stroke)
        
        let drawMask: CGImageRef = CGBitmapContextCreateImage(UIGraphicsGetCurrentContext())!
        UIGraphicsEndImageContext()
        
        CGContextSaveGState(ctx)
        CGContextClipToMask(ctx, bounds, drawMask)
        
        //Gradient - Fill
        if colorsArray.count > 1 {
            var componentsArray: [CGFloat] = []
            let rgbColorsArray: [UIColor] = colorsArray.map {c in // Make sure every color in colors array is in RGB color space
                if CGColorGetNumberOfComponents(c.CGColor) == 2 {
                    let whiteValue = CGColorGetComponents(c.CGColor)[0]
                    return UIColor(red: whiteValue, green: whiteValue, blue: whiteValue, alpha: 1.0)
                } else {
                    return c
                }
            }
            
            for color in rgbColorsArray {
                let colorComponents: UnsafePointer<CGFloat> = CGColorGetComponents(color.CGColor)
                componentsArray.appendContentsOf([colorComponents[0],colorComponents[1],colorComponents[2],1.0])
            }
            
            drawGradientWithContext(ctx, componentsArray: componentsArray)
        } else {
            if colorsArray.count == 1 {
                fillRectWithContext(ctx, color: colorsArray[0])
            } else {
                fillRectWithContext(ctx, color: UIColor(red: 1.0, green: 1.0, blue: 1.0, alpha: 1.0))
            }
        }
        CGContextRestoreGState(ctx)
        UIGraphicsPopContext()
    }
    
    private func fillRectWithContext(ctx: CGContext!, color: UIColor) {
        CGContextSetFillColorWithColor(ctx, color.CGColor)
        CGContextFillRect(ctx, bounds)
    }
    
    private func drawGradientWithContext(ctx: CGContext!, componentsArray: [CGFloat]) {
        let baseSpace = CGColorSpaceCreateDeviceRGB()
        let locations = locationsCache ?? gradientLocationsFromColorCount(componentsArray.count/4, gradientWidth: bounds.size.width)
        let gradient: CGGradient
        
        if let g = self.gradientCache {
            gradient = g
        } else {
            guard let g = CGGradientCreateWithColorComponents(baseSpace, componentsArray, locations,componentsArray.count / 4) else { return }
            self.gradientCache = g
            gradient = g
        }
        
        let halfX = bounds.size.width/2.0
        let floatPi = CGFloat(M_PI)
        let rotateSpeed = clockwise == true ? gradientRotateSpeed : gradientRotateSpeed * -1
        let angleInRadians = ThunderCircularProgressUtils.DegreesToRadians(rotateSpeed * CGFloat(angle) - 90)
        let oppositeAngle = angleInRadians > floatPi ? angleInRadians - floatPi : angleInRadians + floatPi
        
        let startPoint = CGPoint(x: (cos(angleInRadians) * halfX) + halfX, y: (sin(angleInRadians) * halfX) + halfX)
        let endPoint = CGPoint(x: (cos(oppositeAngle) * halfX) + halfX, y: (sin(oppositeAngle) * halfX) + halfX)
        
        CGContextDrawLinearGradient(ctx, gradient, startPoint, endPoint, .DrawsBeforeStartLocation)
    }
    
    private func gradientLocationsFromColorCount(colorCount: Int, gradientWidth: CGFloat) -> [CGFloat] {
        if colorCount == 0 || gradientWidth == 0 {
            return []
        } else {
            var locationsArray: [CGFloat] = []
            let progressLineWidth = radius * progressThickness
            let firstPoint = gradientWidth/2 - (radius - progressLineWidth/2)
            let increment = (gradientWidth - (2*firstPoint))/CGFloat(colorCount - 1)
            
            for i in 0..<colorCount {
                locationsArray.append(firstPoint + (CGFloat(i) * increment))
            }
            assert(locationsArray.count == colorCount, "color counts should be equal")
            let result = locationsArray.map { $0 / gradientWidth }
            locationsCache = result
            return result
        }
    }
    
    private func invalidateGradientCache() {
        gradientCache = nil
        locationsCache = nil
    }
}

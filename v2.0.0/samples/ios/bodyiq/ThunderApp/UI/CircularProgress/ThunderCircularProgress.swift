//
//  ThunderCircularProgress.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 09/12/15.
//  Copyright © 2015 Intel. All rights reserved.
//


import UIKit

public enum ThunderCircularProgressGlowMode {
    case Forward, Reverse, Constant, NoGlow
}

@IBDesignable
public class ThunderCircularProgress: UIView {
    
    private var progressLayer: ThunderCircularProgressViewLayer! {
        get {
            return layer as! ThunderCircularProgressViewLayer
        }
    }
    
    private var radius: CGFloat! {
        didSet {
            progressLayer.radius = radius
        }
    }
    
    @IBInspectable public var angle: Int = 0 {
        didSet {
            if self.isAnimating() {
                self.pauseAnimation()
            }
            progressLayer.angle = angle
        }
    }
    
    @IBInspectable public var startAngle: Int = 0 {
        didSet {
            progressLayer.startAngle = ThunderCircularProgressUtils.Mod(startAngle, range: 360, minMax: (0,360))
            progressLayer.setNeedsDisplay()
        }
    }
    
    @IBInspectable public var clockwise: Bool = true {
        didSet {
            progressLayer.clockwise = clockwise
            progressLayer.setNeedsDisplay()
        }
    }
    
    @IBInspectable public var roundedCorners: Bool = true {
        didSet {
            progressLayer.roundedCorners = roundedCorners
        }
    }
    
    @IBInspectable public var gradientRotateSpeed: CGFloat = 0 {
        didSet {
            progressLayer.gradientRotateSpeed = gradientRotateSpeed
        }
    }
    
    @IBInspectable public var glowAmount: CGFloat = 1.0 {//Between 0 and 1
        didSet {
            progressLayer.glowAmount = ThunderCircularProgressUtils.Clamp(glowAmount, minMax: (0, 1))
        }
    }
    
    @IBInspectable public var glowMode: ThunderCircularProgressGlowMode = .Forward {
        didSet {
            progressLayer.glowMode = glowMode
        }
    }
    
    @IBInspectable public var progressThickness: CGFloat = 0.4 {//Between 0 and 1
        didSet {
            progressThickness = ThunderCircularProgressUtils.Clamp(progressThickness, minMax: (0, 1))
            progressLayer.progressThickness = progressThickness/2
        }
    }
    
    @IBInspectable public var trackThickness: CGFloat = 0.5 {//Between 0 and 1
        didSet {
            trackThickness = ThunderCircularProgressUtils.Clamp(trackThickness, minMax: (0, 1))
            progressLayer.trackThickness = trackThickness/2
        }
    }
    
    @IBInspectable public var trackColor: UIColor = UIColor.blackColor() {
        didSet {
            progressLayer.trackColor = trackColor
            progressLayer.setNeedsDisplay()
        }
    }
    
    @IBInspectable public var progressInsideFillColor: UIColor? = nil {
        didSet {
            if let color = progressInsideFillColor {
                progressLayer.progressInsideFillColor = color
            } else {
                progressLayer.progressInsideFillColor = UIColor.clearColor()
            }
        }
    }
    
    @IBInspectable public var progressColors: [UIColor]! {
        get {
            return progressLayer.colorsArray
        }
        
        set(newValue) {
            setColors(newValue)
        }
    }
    
    //These are used only from the Interface-Builder. Changing these from code will have no effect.
    //Also IB colors are limited to 3, whereas programatically we can have an arbitrary number of them.
    @objc @IBInspectable private var IBColor1: UIColor?
    @objc @IBInspectable private var IBColor2: UIColor?
    @objc @IBInspectable private var IBColor3: UIColor?
    
    
    private var animationCompletionBlock: ((Bool) -> Void)?
    
    override public init(frame: CGRect) {
        super.init(frame: frame)
        userInteractionEnabled = false
        setInitialValues()
        refreshValues()
        checkAndSetIBColors()
    }
    
    convenience public init(frame:CGRect, colors: UIColor...) {
        self.init(frame: frame)
        setColors(colors)
    }
    
    required public init(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)!
        translatesAutoresizingMaskIntoConstraints = false
        userInteractionEnabled = false
        setInitialValues()
        refreshValues()
    }
    
    public override func awakeFromNib() {
        checkAndSetIBColors()
    }
    
    override public class func layerClass() -> AnyClass {
        return ThunderCircularProgressViewLayer.self
    }
    
    public override func layoutSubviews() {
        super.layoutSubviews()
        radius = (frame.size.width/2.0) * 0.8
    }
    
    private func setInitialValues() {
        radius = (frame.size.width/2.0) * 0.8 //We always apply a 20% padding, stopping glows from being clipped
        backgroundColor = .clearColor()
        setColors(UIColor.whiteColor(), UIColor.redColor())
    }
    
    private func refreshValues() {
        progressLayer.angle = angle
        progressLayer.startAngle = ThunderCircularProgressUtils.Mod(startAngle, range: 360, minMax: (0,360))
        progressLayer.clockwise = clockwise
        progressLayer.roundedCorners = roundedCorners
        progressLayer.gradientRotateSpeed = gradientRotateSpeed
        progressLayer.glowAmount = ThunderCircularProgressUtils.Clamp(glowAmount, minMax: (0, 1))
        progressLayer.glowMode = glowMode
        progressLayer.progressThickness = progressThickness/2
        progressLayer.trackColor = trackColor
        progressLayer.trackThickness = trackThickness/2
    }
    
    private func checkAndSetIBColors() {
        let nonNilColors = [IBColor1, IBColor2, IBColor3].filter { $0 != nil}.map { $0! }
        if nonNilColors.count > 0 {
            setColors(nonNilColors)
        }
    }
    
    public func setColors(colors: UIColor...) {
        setColors(colors)
    }
    
    private func setColors(colors: [UIColor]) {
        progressLayer.colorsArray = colors
        progressLayer.setNeedsDisplay()
    }
    
    public func animateFromAngle(fromAngle: Int, toAngle: Int, duration: NSTimeInterval, relativeDuration: Bool = true, completion: ((Bool) -> Void)?) {
        if isAnimating() {
            pauseAnimation()
        }
        
        let animationDuration: NSTimeInterval
        if relativeDuration {
            animationDuration = duration
        } else {
            let traveledAngle = ThunderCircularProgressUtils.Mod(toAngle - fromAngle, range: 360, minMax: (0, 360))
            let scaledDuration = (NSTimeInterval(traveledAngle) * duration) / 360
            animationDuration = scaledDuration
        }
        
        let animation = CABasicAnimation(keyPath: "angle")
        animation.fromValue = fromAngle
        animation.toValue = toAngle
        animation.duration = animationDuration
        animation.delegate = self
        angle = toAngle
        animationCompletionBlock = completion
        
        progressLayer.addAnimation(animation, forKey: "angle")
    }
    
    public func animateToAngle(toAngle: Int, duration: NSTimeInterval, relativeDuration: Bool = true, completion: ((Bool) -> Void)?) {
        if isAnimating() {
            pauseAnimation()
        }
        animateFromAngle(angle, toAngle: toAngle, duration: duration, relativeDuration: relativeDuration, completion: completion)
    }
    
    public func pauseAnimation() {
        let presentationLayer = progressLayer.presentationLayer() as! ThunderCircularProgressViewLayer
        let currentValue = presentationLayer.angle
        progressLayer.removeAllAnimations()
        animationCompletionBlock = nil
        angle = currentValue
    }
    
    public func stopAnimation() {
        progressLayer.removeAllAnimations()
        angle = 0
    }
    
    public func isAnimating() -> Bool {
        return progressLayer.animationForKey("angle") != nil
    }
    
    override public func animationDidStop(anim: CAAnimation, finished flag: Bool) {
        if let completionBlock = animationCompletionBlock {
            completionBlock(flag)
            animationCompletionBlock = nil
        }
    }
    
    public override func didMoveToWindow() {
        if let window = window {
            progressLayer.contentsScale = window.screen.scale
        }
    }
    
    public override func willMoveToSuperview(newSuperview: UIView?) {
        if newSuperview == nil && isAnimating() {
            pauseAnimation()
        }
    }
    
    public override func prepareForInterfaceBuilder() {
        setInitialValues()
        refreshValues()
        checkAndSetIBColors()
        progressLayer.setNeedsDisplay()
    }
}
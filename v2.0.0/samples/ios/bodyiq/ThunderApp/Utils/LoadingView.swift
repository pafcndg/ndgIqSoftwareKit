//
//  LoadingView.swift
//  ThunderApp
//
//  Created by PabloX Garcia-morato on 11/29/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

class CircularLoaderView: UIView {
    
    
    // MARK: Members
    
    let circlePathLayer         = CAShapeLayer()
    let circleRadius: CGFloat   = 30.0
    
    var count:CGFloat = 0.0
    var timer:NSTimer?
    
    var progress: CGFloat {
        get {
            return circlePathLayer.strokeEnd
        }
        set {
            if (newValue > 1) {
                circlePathLayer.strokeEnd = 1
            }
            else if (newValue < 0) {
                circlePathLayer.strokeEnd = 0
            }
            else {
                circlePathLayer.strokeEnd = newValue
            }
        }
    }
    
    
    // MARK: Init
    
    override init(frame: CGRect) {
        
        super.init(frame: frame)
        configure()
    }
    
    required init(coder aDecoder: NSCoder) {
        
        super.init(coder: aDecoder)!
        configure()
    }
    
    
    // MARK: Setup
    
    func configure() {
        
        circlePathLayer.frame       = bounds
        circlePathLayer.lineWidth   = 5
        circlePathLayer.fillColor   = UIColor.clearColor().CGColor
        circlePathLayer.strokeColor = UIColor.redColor().CGColor
        layer.addSublayer(circlePathLayer)
        backgroundColor             = UIColor.clearColor()
        progress = 0
    }
    
    func circleFrame() -> CGRect {
        
        var circleFrame         = CGRect(x: 0, y: 0, width: 2*circleRadius, height: 2*circleRadius)
        circleFrame.origin.x    = CGRectGetMidX(circlePathLayer.bounds) - CGRectGetMidX(circleFrame)
        circleFrame.origin.y    = CGRectGetMidY(circlePathLayer.bounds) - CGRectGetMidY(circleFrame)
        
        return circleFrame
    }
    
    func circlePath() -> UIBezierPath {
        return UIBezierPath(ovalInRect: circleFrame())
    }
    
    override func layoutSubviews() {
        
        super.layoutSubviews()
        circlePathLayer.frame   = bounds
        circlePathLayer.path    = circlePath().CGPath
    }
    override func animationDidStart(anim: CAAnimation) {
        
    }
    override func animationDidStop(anim: CAAnimation, finished flag: Bool) {
        superview?.layer.mask = nil
    }
    
    
    // MARK: Counter
    func start() {
        
        timer = NSTimer.scheduledTimerWithTimeInterval(0.04, target: self, selector: Selector("update"), userInfo: nil, repeats: true)
    }
    
    func stop() {
        
        timer?.invalidate()
        timer = nil
    }
    
    
    func update() {
        
        assert(count >= 0)
        
        if(count < 100)
        {
            count++
        }
        else {
            count = 0
        }
        
        print(count)
        
        progress = count / 100
    }
}
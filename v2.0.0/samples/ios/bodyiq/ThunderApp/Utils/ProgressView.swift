//
//  ProgressView.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/29/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit

struct ProgressViewConfig {
    
    var progressLabelColor: UIColor
    var progressLabelText: String
    var progressLabelFontName: String
    var progressViewStrokeColor: UIColor
    var progressViewLineWidth: CGFloat
    var animationDuration: NSTimeInterval
}

typealias AnimationDidStop = () -> ()

class ProgressView: UIView {
    
    
    // MARK: Members
    
    private let progressLayer: CAShapeLayer = CAShapeLayer()
    private var progressLabel: UILabel?
    private var config: ProgressViewConfig!
    private var stopCallback: AnimationDidStop?
    
    
    // MARK: Init/Deinit
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        defaultConfig()
    }
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        defaultConfig()
    }
    
    init(frame: CGRect, config: ProgressViewConfig) {
        
        super.init(frame: frame)
        self.config = config
    }
    
    deinit {
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    private func defaultConfig() {
        
        self.config = ProgressViewConfig(progressLabelColor: .whiteColor(),
                                         progressLabelText: "loading".localized,
                                         progressLabelFontName: "HelveticaNeue-UltraLight",
                                         progressViewStrokeColor: UIColor.init(hex: Constants.ButtonsActionBarLinkColor),
                                         progressViewLineWidth: 5,
                                         animationDuration: 1.5)
    }
    
    
    // MARK: Start / Stop
    
    func start() {
        
        dispatch_async(dispatch_get_main_queue()) {
            
            self.createLabel()
            
            self.createProgressLayer()
            
            self.animateProgressView()
        }
    }
    
    func stop(animationDidStop: AnimationDidStop) {
        
        self.stopCallback =  animationDidStop
        
        destroyLabel()
        
        destroyProgressLayer()
        
        destroyProgressView()
    }
    
    
    // MARK: Label
    
    private func createLabel() {
        
        progressLabel                   = UILabel(frame: CGRectMake(0.0, 0.0, CGRectGetWidth(frame), 25.0))
        progressLabel!.textColor        = config.progressLabelColor
        progressLabel!.textAlignment    = .Center
        progressLabel!.font             = UIFont(name: config.progressLabelFontName, size: 20.0)
        progressLabel!.translatesAutoresizingMaskIntoConstraints = false
        addSubview(progressLabel!)
        
        addConstraint(NSLayoutConstraint(item: self, attribute: .CenterX, relatedBy: .Equal, toItem: progressLabel, attribute: .CenterX, multiplier: 1.0, constant: 0.0))
        addConstraint(NSLayoutConstraint(item: self, attribute: .CenterY, relatedBy: .Equal, toItem: progressLabel, attribute: .CenterY, multiplier: 1.0, constant: 0.0))
    }
    
    private func destroyLabel() {
        
        progressLabel?.removeFromSuperview()
        progressLabel = nil
    }
    
    
    // MARK: Progress Layer
    
    private func createProgressLayer() {
        
        let startAngle  = CGFloat(M_PI_2)
        let endAngle    = CGFloat(M_PI * 2 + M_PI_2)
        let centerPoint = CGPointMake(CGRectGetWidth(frame)/2 , CGRectGetHeight(frame)/2)
        
        progressLayer.path = UIBezierPath(arcCenter:centerPoint, radius: CGRectGetWidth(frame)/2 + 1.0, startAngle:startAngle, endAngle:endAngle, clockwise: true).CGPath
        progressLayer.backgroundColor   = UIColor.clearColor().CGColor
        progressLayer.fillColor         = nil
        progressLayer.strokeColor       = config.progressViewStrokeColor.CGColor
        progressLayer.lineWidth         = config.progressViewLineWidth
        progressLayer.strokeStart       = 0.0
        progressLayer.strokeEnd         = 0.0
        
        layer.addSublayer(progressLayer)
    }
    
    private func destroyProgressLayer() {
        
        progressLayer.removeFromSuperlayer()
    }
    
    
    // MARK: Progress View
    
    private func animateProgressView() {
        
        progressLabel?.text             = config.progressLabelText
        progressLayer.strokeEnd         = 0.0
        
        let animation                   = CABasicAnimation(keyPath: "strokeEnd")
        animation.fromValue             = CGFloat(0.0)
        animation.toValue               = CGFloat(1.0)
        animation.duration              = config.animationDuration
        animation.delegate              = self
        animation.additive              = false
        animation.cumulative            = false
        animation.autoreverses          = false
        animation.repeatCount           = Float.infinity
        animation.timingFunction        = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        animation.fillMode              = kCAFillModeForwards
        animation.removedOnCompletion   = true
        
        progressLayer.addAnimation(animation, forKey: "strokeEnd")
    }
    
    func destroyProgressView() {
        progressLayer.strokeEnd = 0.0
        progressLayer.removeAllAnimations()
    }
    
    override func animationDidStop(anim: CAAnimation, finished flag: Bool) {
        stopCallback?()
    }
}
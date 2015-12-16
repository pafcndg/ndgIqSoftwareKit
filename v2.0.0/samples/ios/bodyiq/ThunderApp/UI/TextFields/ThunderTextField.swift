//
//  ThunderTextField.swift
//  TextFieldEffects
//
//  Created by Pablo Garcia-Morato on 01/12/2015.
//  Copyright (c) 2015 Intel. All rights reserved.
//

import UIKit


typealias DidBeginEditing               = () -> ()
typealias DidEndEditing                 = () -> ()
typealias TextEntryAnimationComplete    = (Bool) -> ()
typealias TextDisplayAnimationComplete  = (Bool) -> ()


/// Custom UITextField with visual effect around the control's lower edge
@IBDesignable class ThunderTextField: UITextField {
    

    // MARK: IBInspectable
    
    /// Lower edge color when there's no content. Clear color by default.
    @IBInspectable dynamic var borderInactiveColor: UIColor? {
        didSet {
            updateBorder()
        }
    }
    
    /// Lower edge color when there's content. Clear color by default.
    @IBInspectable dynamic var borderActiveColor: UIColor? {
        didSet {
            updateBorder()
        }
    }
    
    // Text placeholder text. Gray by default
    @IBInspectable dynamic var placeholderColor: UIColor = .grayColor() {
        didSet {
            updatePlaceholder()
        }
    }
    
    // Place holder label size relative to the text field font size
    @IBInspectable dynamic var placeholderFontScale: CGFloat = 0.65 {
        didSet {
            updatePlaceholder()
        }
    }

    override var placeholder: String? {
        didSet {
            updatePlaceholder()
        }
    }
    
    override var bounds: CGRect {
        didSet {
            updateBorder()
            updatePlaceholder()
        }
    }
    
    
    // MARK: Members
    
    let placeholderLabel = UILabel()
    
    private let borderThickness: (active: CGFloat, inactive: CGFloat) = (active: 2, inactive: 0.5)
    private let placeholderInsets               = CGPoint(x: 0, y: 6)
    private let textFieldInsets                 = CGPoint(x: 0, y: 12)
    private let inactiveBorderLayer             = CALayer()
    private let activeBorderLayer               = CALayer()
    private var activePlaceholderPoint: CGPoint = CGPointZero
    
    private var texdDidBeginEditing: DidBeginEditing?
    private var textDidEndEditing: DidEndEditing?
    private var textDisplayAnimationComplete: TextDisplayAnimationComplete?
    private var textEntryAnimationComplete: TextEntryAnimationComplete?
    
    
    // MARK: - Overrides
    
    override func drawRect(rect: CGRect) {
        drawViewsForRect(rect)
    }
    
    override func drawPlaceholderInRect(rect: CGRect) {
        // Don't draw any placeholders
    }
    
    override var text: String? {
        didSet {
            if let text = text where !(text.isEmpty) {
                animateViewsForTextEntry()
            }
            else {
                animateViewsForTextDisplay()
            }
        }
    }
    
    
    // MARK: - TextFieldsEffects
    
    func drawViewsForRect(rect: CGRect) {
        
        let frame = CGRect(origin: CGPointZero, size: CGSize(width: rect.size.width, height: rect.size.height))
        
        placeholderLabel.frame  = CGRectInset(frame, placeholderInsets.x, placeholderInsets.y)
        placeholderLabel.font   = placeholderFontFromFont(font!)
        
        updateBorder()
        updatePlaceholder()
        
        layer.addSublayer(inactiveBorderLayer)
        layer.addSublayer(activeBorderLayer)
        
        addSubview(placeholderLabel)
    }
    
    func animateViewsForTextEntry() {
        
        if text!.isEmpty {
            
            UIView.animateWithDuration(0.3,
                                delay: 0.0,
                                usingSpringWithDamping: 0.8,
                                initialSpringVelocity: 1.0,
                                options: .BeginFromCurrentState,
                                animations: ({
                                    self.placeholderLabel.frame.origin = CGPoint(x: 10, y: self.placeholderLabel.frame.origin.y)
                                    self.placeholderLabel.alpha = 0
                                }),
                                completion: { [weak self] b in
                                    self?.textEntryAnimationComplete?(b)
                                })
        }
    
        layoutPlaceholderInTextRect()
        
        placeholderLabel.frame.origin = activePlaceholderPoint
        
        UIView.animateWithDuration(0.2, animations: {
            self.placeholderLabel.alpha = 0.5
        })
        
        activeBorderLayer.frame = self.rectForBorder(self.borderThickness.active, isFilled: true)
    }
    
    func animateViewsForTextDisplay() {
        
        if text!.isEmpty {
            
            UIView.animateWithDuration(0.35,
                                delay: 0.0,
                                usingSpringWithDamping: 0.8,
                                initialSpringVelocity: 2.0,
                                options: UIViewAnimationOptions.BeginFromCurrentState,
                                animations: ({ [unowned self] in
                                    self.layoutPlaceholderInTextRect()
                                    self.placeholderLabel.alpha = 1
                                }),
                                completion: { [weak self] b in
                                    self?.textDisplayAnimationComplete?(b)
                                })
            
            self.activeBorderLayer.frame = self.rectForBorder(self.borderThickness.active, isFilled: false)
        }
    }
    
    
    // MARK: - Private
    
    func updateBorder() {
        
        inactiveBorderLayer.frame           = rectForBorder(borderThickness.inactive, isFilled: true)
        inactiveBorderLayer.backgroundColor = borderInactiveColor?.CGColor
        
        activeBorderLayer.frame             = rectForBorder(borderThickness.active, isFilled: false)
        activeBorderLayer.backgroundColor   = borderActiveColor?.CGColor
    }
    
    func updatePlaceholder() {
    
        placeholderLabel.text       = placeholder
        placeholderLabel.textColor  = placeholderColor
        placeholderLabel.sizeToFit()
        layoutPlaceholderInTextRect()
        
        if isFirstResponder() || !(text!.isEmpty) {
            animateViewsForTextEntry()
        }
    }
    
    func placeholderFontFromFont(font: UIFont) -> UIFont! {
        
        let smallerFont = UIFont(name: font.fontName, size: font.pointSize * placeholderFontScale)
        return smallerFont
    }
    
    func rectForBorder(thickness: CGFloat, isFilled: Bool) -> CGRect {
        
        if isFilled {
            return CGRect(origin: CGPoint(x: 0, y: CGRectGetHeight(frame)-thickness), size: CGSize(width: CGRectGetWidth(frame), height: thickness))
        }
        else {
            return CGRect(origin: CGPoint(x: 0, y: CGRectGetHeight(frame)-thickness), size: CGSize(width: 0, height: thickness))
        }
    }
    
    func layoutPlaceholderInTextRect() {
        
        let textRect = textRectForBounds(bounds)
        var originX  = textRect.origin.x
        
        switch self.textAlignment {
            case .Center:
                originX += textRect.size.width/2 - placeholderLabel.bounds.width/2
            case .Right:
                originX += textRect.size.width - placeholderLabel.bounds.width
            default:
                break
        }
        
        placeholderLabel.frame = CGRect(x: originX,
                                        y: textRect.height/2,
                                        width: placeholderLabel.bounds.width,
                                        height: placeholderLabel.bounds.height)
        
        activePlaceholderPoint = CGPoint(x: placeholderLabel.frame.origin.x,
                                         y: placeholderLabel.frame.origin.y - placeholderLabel.frame.size.height - placeholderInsets.y)

    }
    
    
    // MARK: - UITextField Observing
    
    override func willMoveToSuperview(newSuperview: UIView!) {
        if newSuperview != nil {
            NSNotificationCenter.defaultCenter().addObserver(self, selector: "textFieldDidEndEditing", name:UITextFieldTextDidEndEditingNotification, object: self)
            
            NSNotificationCenter.defaultCenter().addObserver(self, selector: "textFieldDidBeginEditing", name:UITextFieldTextDidBeginEditingNotification, object: self)
        }
        else {
            NSNotificationCenter.defaultCenter().removeObserver(self)
        }
    }
    
    func textFieldDidBeginEditing() {
        
        animateViewsForTextEntry()
        texdDidBeginEditing?()
    }
    
    func textFieldDidEndEditing() {
        
        animateViewsForTextDisplay()
        textDidEndEditing?()
    }
    
    
    // MARK: - Interface Builder
    
    /**
    This method compiles with the rest of the code but is only executed when our view is being prepared for display in Interface Builder.
    */
    override func prepareForInterfaceBuilder() {
        drawViewsForRect(frame)
    }
    
    
    // MARK: Listener
    
    /**
    Events listener
    
    - parameter texdDidBeginEditing:          Called when begin editing
    - parameter textDidEndEditing:            Called when end editing
    - parameter textDisplayAnimationComplete: Called when text display animation is completed
    - parameter textEntryAnimationComplete:   Called when text entry animation is completed
    */
    func addEventsListener(texdDidBeginEditing: DidBeginEditing,
        textDidEndEditing: DidEndEditing,
        textDisplayAnimationComplete: TextDisplayAnimationComplete,
        textEntryAnimationComplete: TextEntryAnimationComplete) {
            
            self.texdDidBeginEditing = texdDidBeginEditing
            self.textDidEndEditing = textDidEndEditing
            self.textDisplayAnimationComplete = textDisplayAnimationComplete
            self.textEntryAnimationComplete = textEntryAnimationComplete
    }
    
    
    // MARK: - Overrides
    
    override func editingRectForBounds(bounds: CGRect) -> CGRect {
        return CGRectOffset(bounds, textFieldInsets.x, textFieldInsets.y)
    }
    
    override func textRectForBounds(bounds: CGRect) -> CGRect {
        return CGRectOffset(bounds, textFieldInsets.x, textFieldInsets.y)
    }
}
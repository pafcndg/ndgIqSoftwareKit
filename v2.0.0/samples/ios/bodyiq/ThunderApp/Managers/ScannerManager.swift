//
//  ScannerManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/10/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import UIKit
import IQCore

public typealias WearableTokenArray = [WearableScannerTokenResult]
public typealias WearableTokens     = (tokens: WearableTokenArray) -> ()

/// Class to look for new BLE devices
class ScannerManager {
    
    // MARK: Members
    
    private var tokens = WearableTokenArray()
    private var scanner: WearableScannerType!
    private var periferal: PeriferalManager = PeriferalManager()
    private weak var view: UIView?
    
    
    // MARK: Init / Deinit

    /**
    Default ScannerManager initializer
    
    - returns: ScannerManager object
    */
    init(view: UIView) {
        
        scanner     = WearableScanner.scannerForProtocol(.BLE)
        self.view   = view
    }
    
    init() {
        
        fatalError("Initialize with init(view: UIView)")
    }
    
    deinit {
        
        destroy()
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    func destroy() {
        
        if let v = view?.superview?.superview {
            
            removeLoading(v, completion: nil)
        }
        
        scanner     = nil
    }
    
    
    // MARK: Start / Stop Scanner
    
    func startScan(tokens: WearableTokens) {
        
        // Check if bluetooth is available before start scanning
        
        guard !scanner.isScanning else {
            DLog("Already scanning")
            return
        }
            
        if periferal.isEnabled() {
            
            // Add Loading
            if let v = view {
             addLoading(v, message: "")
            }
            
            // Start scanning
            scanner.startScan({ [weak self] (token) -> () in
                
                //DLog("found: \(token.displayName)")
                
                // Add found token to the tokens array
                self?.tokens.filterAppend(token)
                
                // Update tokens array
                if let arr = self?.tokens {
                    
                    tokens(tokens: arr)
                }
                
                }, error: { [weak self] (error) -> () in
                    
                    self?.stopScan()
                    
                    DLog("\(error)")
                    
                }, finish: { [weak self] () -> () in
                    
                    self?.stopScan()
                    
                    DLog("finished")
                    
                })
            
        }//isEnabled
        else {
            
            // Wait for the user to enable the bluetooth...
            // Add Loading
            
            if let v = view?.superview?.superview {
                
                addLoading(v, message: "connect_ble".localized)
                
                
                // Subscribe to bluetooth status updates and try to scan again when it is available
                periferal.stateUpdate( { [weak self] (state) -> () in
                    
                    if state == .PoweredOn
                    {
                        self?.removeLoading(v, completion: { () -> Void in
                            
                            self?.startScan(tokens)
                            
                        })
                    }
                })
            }
        }//else
    }
    
    func stopScan() {
        
        guard scanner.isScanning else {
            DLog("Already stopped scanning")
            return
        }

        
        if let v = view {
            
            removeLoading(v, completion: { [weak self]() -> Void in
                
                DLog("Loading completely removed")
                
                // Stop scanning
                self?.scanner.stopScan()
                
            })
        }
    }
    
    
    // MARK: Add / Remove loading
    
    private func addLoading(view: UIView, message: String) {
        
        do {
            try Loading.sharedInstance.addLoading(view, mode: .Indeterminate, label: message)
        }
        catch LoadingError.LoadingInstanceAlreadyAdded {
            DLog("Already scanning")
        }
        catch {
            ALog("Something went wrong")
        }
        
    }
    
    private func removeLoading(view: UIView, completion: LoadingCompletionBlock?) {
        
        // Remove loading
        do {
            try Loading.sharedInstance.removeLoading(view, completion:completion)
        }
        catch LoadingError.LoadingInstanceAlreadyAdded {
            DLog("Already stopped scanner")
        }
        catch {
            ALog("Something went wrong")
        }
    }
}

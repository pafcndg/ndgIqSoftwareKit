//
//  FirmwareManager.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 25/11/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import IQCore
import CDKCore
import CDKBlobStore

struct CloudFirmwareInfo {
    
    var assetId: String
    var productId: String
    var version: String
    var path: String?
    
    init(assetId: String, productId: String, version: String) {
        self.assetId = assetId
        self.productId = productId
        self.version = version
    }
}


typealias firmwareRevision          = (String) -> ()
typealias firmwareInfo              = (CloudFirmwareInfo) -> ()
typealias firmwareError             = (NSError) -> ()
typealias firmwareDownloadProgress  = (Float) -> ()
typealias firmwareInstallProgress   = (Float) -> ()


private let DefaultTimeoutLengthInNanoSeconds: Int64 = 10000000000 // 10 Seconds 

class FirmwareManager {
    
    
    // MARK: Members
    
    static let sharedInstance = FirmwareManager()
    private var firmwareController: FirmwareControllerType?
    private var blobStore: CloudPublicBlobStore?
    
    private(set) var latestCloudFirmwareInfo: CloudFirmwareInfo?
    
    
    // MARK: Init / Deinit
    
    private init() {
        
        guard let controller = GlobalStorage.controller?.controller else {
            
            fatalError("Controller should be already initiated at this point.")
        }
        
        firmwareController  = FirmwareController.controllerForWearable(controller)
        
        initCloud()
    }
    
    deinit {
        
        firmwareController              = nil
        blobStore                       = nil
        latestCloudFirmwareInfo         = nil
        
        // TODO: Remove...
        DLog("\(__FUNCTION__) called")
    }
    
    
    // MARK: Config CDK Connection
    
    /**
    Setup cloud configuration
    */
    private func initCloud() {
        
        let authProvider = CloudAppAuthProvider.init(baseURL: Constants.BaseUAA2URL,
                                                     clientId: Constants.ClientId,
                                                     clientSecret: Constants.ClientSecret,
                                                     authToken: Constants.AuthenticationToken)
        
        authProvider.login()
        
        blobStore = CloudPublicBlobStore(baseUrl: Constants.BaseAppURL,
                                         productId: Constants.ProductId,
                                         provider: authProvider,
                                         doAllowCellularAccess: true)
    }
    
    
    // MARK: Current Firmware
    
    /**
    Get current device firmware
    
    - parameter firmware: Firmware provided via closure
    */
    func deviceFirmware() -> String {
        
        return GlobalStorage.deviceDescription?.firmwareRevision ?? "Not Found"
    }
    
    
    // MARK: Available Firmware
    
    /**
     Connect to the cloud and retrive latest firmware available
     
     - parameter firmware: CloudFirmwareInfo object provided via closure
     */
    func checkAvailableFirmware(firmware: firmwareInfo, error: firmwareError) {
        
        blobStore?.getLatestDownloadInfo(Constants.Firmware,
            
            success: { cloudResponse in
            
                let JSONPayload = cloudResponse.getJsonPayload()
                
                guard let assetInfo = JSONPayload[Constants.AssetInfo] as? NSDictionary else {
                    
                    error(NSError.init(localizedDescription: "Couldn't get JSON payload", localizedFailureReason: "", domain: "com.intel.CDKCore"))
                    
                    return
                }
                
                DLog("Available firmware: \(assetInfo)")
                
                guard let assetId   = assetInfo[Constants.AssetId] as? String,
                      let productId = assetInfo[Constants.FProductId] as? String,
                      let version   = assetInfo[Constants.AssetVersion] as? String else {
                        
                        error(NSError.init(localizedDescription: "No Firmware Found", localizedFailureReason: "", domain: "com.intel.CDKCore"))
                        
                        return
                }
            
                let firmwareInfo = CloudFirmwareInfo(assetId: assetId, productId: productId, version: version)
            
                // Save latest firmware info
                self.latestCloudFirmwareInfo = firmwareInfo
                
                firmware(firmwareInfo)
            
            },
            failure: { response in
                error(NSError.init(localizedDescription: "Latest firmware check failed", localizedFailureReason: "\(response.getCloudError)", domain: "com.intel.CDKCore"))
        })
    }
    
    
    // MARK: Download Firmware
    
    func downloadFirmware(success: IBlobDownloadStartedCb,
                          failure: IBlobDownloadStartedCb,
                          progress: firmwareDownloadProgress,
                          completed: IBlobDownloadFinishedCb ) {
            
            guard let firmwareAssetId = latestCloudFirmwareInfo?.assetId else {
                
                assertionFailure("No asset id")
                return
            }
                            
            // Always force a download by removing file if it was previously downloaded
            // TODO: user FileManager custom class
            let fileManager = NSFileManager.defaultManager()
            let docDirPath  = NSSearchPathForDirectoriesInDomains(.DocumentDirectory, .UserDomainMask, true).first!
            let docPath     = docDirPath + "/" + firmwareAssetId
            
            if fileManager.fileExistsAtPath(docPath) {
                do {
                    try fileManager.removeItemAtPath(docPath)
                    
                } catch let error as NSError {
                    DLog("Could not remove existing firmware file: \(docPath): error: \(error.description)")
                }
            }
            
            self.blobStore?.download(firmwareAssetId, saveAsFile: firmwareAssetId,
                
                success: { cloudResponse in
                    
                    success(cloudResponse)
                                        
                }, failure: { cloudResponse in
                    
                    failure(cloudResponse)
                    
                }, progress: { bytesWritten, totalBytesWritten, totalBytesExpectedToWrite in
                    
                    let complete = (Float)(totalBytesWritten)/(Float)(totalBytesExpectedToWrite)
                    
                    let percentComplete = complete
                    
                    progress(percentComplete)
                    
                }, completed: { [weak self] cloudResponse in
                    
                    let docDirPath  = NSSearchPathForDirectoriesInDomains(.DocumentDirectory, .UserDomainMask, true).first!
                    let docPath     = docDirPath + "/" + firmwareAssetId
                    self?.latestCloudFirmwareInfo?.path = docPath
                    
                    completed(cloudResponse)
                })
    }
    
    private func firmwareDocPath(firmwareAssetId: String) -> String {
        
        return FileManager.getPath(firmwareAssetId, directory: .DocumentDirectory)
    }
    
    private func removeFirmwareFile(path: String) {
        
        FileManager.removeFile(path)
    }
    
    
    // MARK: Install Firmware
    
    func installFirmware(started: FirmwareInstallStarted,
                         progress: firmwareInstallProgress,
                         completed: FirmwareInstallCompleted,
                         failed: FirmwareInstallFailed) {
        
        guard let path = latestCloudFirmwareInfo?.path else {
            assertionFailure("Path to firmware is nil")
            return
        }
        
        firmwareController?.installFirmware(path,
            
            started: { () in
            
                started()
            
            }, progress: { bytesSent, bytesTotal in
                
                let complete = (Float)(bytesSent)/(Float)(bytesTotal)
                
                let percentComplete = complete * 100.0
                
                progress(percentComplete)
                
            }, completed: { [weak self] version in
                
                defer {
                    completed(version: version)
                }
                
                self?.removeFirmwareFile(path)
                
            }, failed: { error in
                
                failed(error: error)
                
        })
    }
    
    
    // MARK: Helper
    
    class func isNewFirmware(currentVersion: String, newVersion: String) -> Bool {
        
        let version = newVersion.componentsSeparatedByString(".")
        
        let v       = currentVersion.substringWithRange(Range(start: (currentVersion.endIndex.advancedBy(-4)), end: currentVersion.endIndex))
        
        print("\(version.last) vs \(v)")
        
        return version.last != v
    }
}
//
//  BodyIQRefAppTests.swift
//  BodyIQRefAppTests
//
//  Created by Pablo Garcia-Morato on 11/6/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import XCTest
import IQCore
@testable import BodyIQRefApp

enum XCTimeout: NSTimeInterval {
    case XCTimeoutShort     = 10
    case XCTimeoutMedium    = 30
    case XCTimeoutLong      = 60
}

class BodyIQRefAppTests: XCTestCase {
    
    static var scanner:ScannerManager?
    static var token:WearableToken?
    static var controller:ControllerManager?
    static var user:UserManager?
    static var ts:TimeSeriesManager?
    static var firmware:FirmwareManager?
    
    let USER_ID:String = "user-id"
    
    override func setUp() {
        super.setUp()
        // Put setup code here. This method is called before the invocation of each test method in the class.        
    }
    
    override func tearDown() {
        // Put teardown code here. This method is called after the invocation of each test method in the class.
        super.tearDown()
    }
    
    /**
     Test scanner init
     */
    func testAAInit() {
        
        let expectation = expectationWithDescription("Check bluetooth...")
        
        let periferal = PeriferalManager()
        
        periferal.stateUpdate( { (state) -> () in
            
            if state == .PoweredOn
            {
                BodyIQRefAppTests.scanner = ScannerManager()
                
                expectation.fulfill()
            }
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutShort.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Bluetooth ON")
        }
    }
    
    /**
     Test scan and find Thunder devices
     */
    func testABScanner() {
        
        let expectation = expectationWithDescription("Start Scanner...")
        
        // Scan
        BodyIQRefAppTests.scanner?.startScan( { (tokens) -> () in
            
            if let str = tokens.last?.token.displayName {
                
                DLog("found: \(str)")
                
                // Connect to the first Thunder device we find
                if str.lowercaseString.rangeOfString("thunder") != nil {
                    
                    XCTAssertNotNil( tokens.last )
                    
                    BodyIQRefAppTests.token = tokens.last?.token
                    
                    expectation.fulfill()
                }
            }
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutShort.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Finish scan...")
        }
    }
    
    /**
     Test connect to a device and get its info
     */
    func testACConnect() {
        
        XCTAssertNotNil( BodyIQRefAppTests.token, "Token should have been initiated on testABScanner()")
        
        let expectation = expectationWithDescription("Start Connecting...")
        
        // Connect
        let wearableController = WearableController.controllerForToken(BodyIQRefAppTests.token!)
        
        BodyIQRefAppTests.controller  = ControllerManager(token: wearableController, observer:{ [weak self] (status) -> () in
            
            if let _ = self {
                if status == .Connected {
                    
                    print("Connected...")
                    
                    BodyIQRefAppTests.controller?.deviceDescription( { (deviceDescription) -> () in
                        
                        DLog("Device info: \n Description: \(deviceDescription?.uuid)" +
                            "\n Name: \(deviceDescription?.displayName) " +
                            "\n Manufacturer: \(deviceDescription?.manufacturerName) \n ModelNum: \(deviceDescription?.modelNumber) " +
                            "\n Serial: \(deviceDescription?.serialNumber) \n Firmware: \(deviceDescription?.firmwareRevision) " +
                            "\n SoftRev: \(deviceDescription?.softwareRevision) \n HardwareRev: \(deviceDescription?.hardwareRevision)")
                    })
                    
                    expectation.fulfill()
                }
            }
        })
        
        // FIXME: The is a bug in IQCore. You can not get the WearableControllerType and connect straigth. The device won't be ready.
        //sleep(1)
        
        BodyIQRefAppTests.controller?.connectToController( { (state) -> () in

        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutShort.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
        }
    }
    
    /**
     Test send notifications
     */
    func testADSendNotifications() {
        
        XCTAssertNotNil( BodyIQRefAppTests.controller, "Controller should have been initiated on testACConnect()")
        
        let expectation = expectationWithDescription("Start Sending Notifications...")
        
        //dispatch_async(dispatch_get_main_queue()) {
            
            // Send notification
            let _ = SendVibrationLedNotification(controller: BodyIQRefAppTests.controller, completion: { () -> () in
                
                // FIXME: IQCore bug. sendNotification closures won't work. Therefore we are not notified when the notification has been sent.
                sleep(10)
                expectation.fulfill()
            })
        //}
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutMedium.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Notification sent...")
        }
    }
    
    /**
     Test create user
     */
    func testAECreateUser() {
        
        // In order to run this test alone we need to init + scan + connect to the device
        /*
        testAAInit()
        testABScanner()
        testACConnect()
        */
        
        BodyIQRefAppTests.user = UserManager()
        
        let user = ThunderUser(identifier: USER_ID, firstName: "Pablo", lastName: "GM", email: "invanzert@gmail.com", phoneNumber: nil, weight_kg: 75, height_cm: 180, biologicalSex: .Male)
        
        BodyIQRefAppTests.user?.createUser(user)
        
        
        // As we run this test stand alone we manually call the retrieve and release
        /*
        testAFRetrieveUser()
        testAHRelease()
        */
    }
    
    /**
     Test retrieve user
     */
    func testAFRetrieveUser() {
        
        // Test persistency
        // FIXME: persistency not working
        /*
        testAAInit()
        testABScanner()
        testACConnect()
        */
        
        let user = BodyIQRefAppTests.user?.getUserForIdentifier(USER_ID)
        
        print("********************* \n" +
              "id: \(user?.identifier) \n" +
              "firstName: \(user?.firstName) \n" +
              "lastName: \(user?.lastName) \n" +
              "email: \(user?.email) \n" +
              "phoneNumber: \(user?.phoneNumber) \n" +
              "weight_kg: \(user?.weight_kg) \n" +
              "height_cm: \(user?.height_cm) \n" +
              "sex: \(user?.biologicalSex == .Male ? "Male" : "Female") \n" +
              "********************* \n")
    }
    
    /**
     Test query time series
     */
    func testAGQueryTimeSeries() {
        
        testAAInit()
        testABScanner()
        testACConnect()
        testAECreateUser()
        
        
        let expectation = expectationWithDescription("Query time series...")
        
        let now = NSDate()
        
        let tenDaysAgo = NSDate.dateBySubstractingDays(10)
                
        BodyIQRefAppTests.ts = TimeSeriesManager(startDate: tenDaysAgo, endDate: now)
        
        BodyIQRefAppTests.ts?.timeSeriesQuery( { data in
        
            XCTAssert( data.count > 0, "Time series query with no results" )
            
            BodyIQRefAppTests.ts?.extractTimeSeriesData(data)
            
            expectation.fulfill()
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutMedium.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Query done...")
        }
        
        
        testALRelease()
    }
    
    /**
     Test get current device firmware
     */
    func testAHGetCurrentDeviceFirmware() {
        
        testAAInit()
        testABScanner()
        testACConnect()
        
        
        if let _ = BodyIQRefAppTests.controller?.controller {
            
            BodyIQRefAppTests.firmware = FirmwareManager()
            
            XCTAssertNotNil(BodyIQRefAppTests.firmware)
            
            let fw = BodyIQRefAppTests.firmware!.deviceFirmware
            
            DLog("Device current firmware \(fw)")
            
            //XCTAssertNotNil(fw)
        }
        else {
            XCTAssert(false, "Controller should have been initiated on testACConnect()")
        }
        
        
        testAIGetLatestAvailableFirmware()
        testAJDownloadFirmware()
        testAKInstallFirmware()
        testALRelease()
    }
    
    /**
     Test get latest firmware available version
     */
    func testAIGetLatestAvailableFirmware() {
        
        let expectation = expectationWithDescription("Query cloud ...")
        
        XCTAssertNotNil(BodyIQRefAppTests.firmware, "Should have been initiated on testAHGetCurrentDeviceFirmware()")
        
        BodyIQRefAppTests.firmware!.latestCloudFirmwareInfo({firmware in
        
            DLog("Latest available firmware \(firmware)")
            
            XCTAssertNotNil(firmware)
            
            expectation.fulfill()
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutMedium.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Query cloud done...")
        }
    }
    
    /**
     Test download firmware
     */
    func testAJDownloadFirmware() {
        
        let expectation = expectationWithDescription("Download firmware ...")
        
        XCTAssertNotNil(BodyIQRefAppTests.firmware, "Should have been initiated on testAHGetCurrentDeviceFirmware()")
        
        BodyIQRefAppTests.firmware!.downloadFirmware( { success in
            
                DLog("Download Start...")
            
            }, failure: {failure in
                
                DLog("Failed to download:  \(failure.getCloudError())")
                
                expectation.fulfill()
                
                XCTAssert(false)
                
            }, progress: {progress in
                
                DLog("Download Percent complete: \(progress)%")
                
            }, completed: {completed in
        
                DLog("Completed download: \(completed.getFileName()))")
                
                expectation.fulfill()
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutMedium.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Firmware downloaded...")
        }
    }
    
    /**
     Test install firmware
     */
    func testAKInstallFirmware() {
        
        let expectation = expectationWithDescription("Installing firmware ...")
        
        XCTAssertNotNil(BodyIQRefAppTests.firmware, "Should have been initiated on testAHGetCurrentDeviceFirmware()")
        
        BodyIQRefAppTests.firmware!.installFirmware( {started in
            
                DLog("Firmware install started")
            
            }, progress: {progress in
                
                DLog("Install Percent complete: \(progress)%")
                
            }, completed: {version in
                
                DLog("Installation completed. Now at version: \(version)")
                
                expectation.fulfill()
                
            }, failed: {error in
        
                DLog("Installation failed: \(error.localizedDescription)")
                
                expectation.fulfill()
                
                XCTAssert(false)
        })
        
        waitForExpectationsWithTimeout(XCTimeout.XCTimeoutMedium.rawValue) { error in
            
            if let error = error {
                print("Error: \(error.localizedDescription)")
            }
            
            print("Firmware installed...")
        }
    }
    
    /**
     Test release
     */
    func testALRelease() {
        
        BodyIQRefAppTests.scanner?.destroy()
        BodyIQRefAppTests.scanner     = nil
        BodyIQRefAppTests.ts          = nil
        BodyIQRefAppTests.token       = nil
        BodyIQRefAppTests.controller  = nil
        BodyIQRefAppTests.user        = nil
        BodyIQRefAppTests.firmware    = nil
    }
}

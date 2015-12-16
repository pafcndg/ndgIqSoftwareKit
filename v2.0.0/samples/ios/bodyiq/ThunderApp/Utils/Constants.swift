//
//  Constants.swift
//  BodyIQRefApp
//
//  Created by Pablo Garcia-Morato on 11/9/15.
//  Copyright © 2015 Intel. All rights reserved.
//

import Foundation

struct Constants {
    
    static let AppName                      = "BodyIQRefApp"
    
    
    // MARK: UI
    static let CellId                       = "ScannerCellId"
    static let MenuCellId                   = "sideMenuCellId"
    static let SessionCellId                = "SessionCellId"
    static let ActivityCellId               = "ActivityCellId"
    static let DeviceDetailsCellId          = "DeviceDetailsCellId"
    static let LoadingToLoginSegue          = "LoadingToLogin"
    static let LoginToCreateAccountnSegue   = "LoginToCreateAccount"
    static let LoginToCredentialsSegue      = "LoginToCredentials"
    static let CreateAccountToSetWeight     = "CreateAccountToSetWeight"
    static let SetWeightToSetHeight         = "SetWeightToSetHeight"
    static let CredentialsToSync            = "CredentialsToSync"
    static let SyncToScanDevices            = "SyncToScanDevices"
    static let ScannerToSidebar             = "ScannerToSidebar"
    static let PushWindowFromSideBar        = "pushWindowFromSideBar"
    static let ModalWindowFromSideBar       = "modalWindowFromSideBar"
    static let DeviceToInfo                 = "deviceToInfo"
    static let DeviceToFirmware             = "deviceToFirmware"
    
    // MARK: Notifications
    static let ConnectionStateNotification   = "connectionStateNotification"
    static let IsConnectionState             = "isConnectionState"
    
    // MARK: Color palette
    static let BackgroundColor              = 0x033960 // R: 3 G: 57 B: 96
    static let ButtonsActionBarLinkColor    = 0x0978BF // R: 9 G: 120 B: 191
    static let StatusBarColor               = 0x0978BF // R: 9 G: 120 B: 191
    static let HighlightActiveTabs          = 0x27ADDC // R: 39 G: 173 B: 220
    static let SecondaryActionColor         = 0xECF5FA // R: 236 G: 245 B: 250
    static let NonFocusedColor              = 0x002B48 // R: 0 G: 43 B: 72
    static let HighlightActiveField         = 0x0e8ac0 // R: 14 G: 138 B: 192
    
    // MARK: Notifications
    static let ToggleMenu                   = "toggleMenu"
    static let CloseMenu                    = "closeMenuViaNotification"
    static let OpenPushWindow               = "openPushWindow"
    static let OpenModalWindow              = "openModalWindow"
    
    // MARK: Firmware
    static let Firmware                     = "Firmware"
    static let AssetInfo                    = "swAssetInfo"
    static let AssetId                      = "assetId"
    static let FProductId                   = "productId"
    static let AssetVersion                 = "assetVersion"
    
    // MARK: CDK Connection
    static let ClientId                     = "ndgThunderDome"
    static let ClientSecret                 = "ndgThunderDomeSecRet"
    static let ProductId                    = "1"
    
    #if DEBUG
    static let BaseUAA2URL         = "https://int-uaa.td.wearables.infra-host.com"
    static let BaseAppURL          = "https://int-app.td.wearables.infra-host.com"
    static let AuthenticationToken = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiJjNjY2ODA0ZS1jYWVlLTQyNWItODc5Mi1mYjBmMDhmYTE0ZjkiLCJzdWIiOiIzODNmODk5YS03NGI2LTRjMmQtODQ1NC00MGI2OGQ2Y2IyZWQiLCJzY29wZSI6WyJhbGwudXNlciJdLCJpYXQiOjE0NDQzNTExMjMsImV4cCI6MTQ3NTkwODcyMywiY2lkIjoibmRnVGh1bmRlckRvbWUiLCJsb2dpbl90eXBlIjoidWFhIiwiaXNzIjoiaHR0cHM6Ly9pbnQtdWFhLnRkLndlYXJhYmxlcy5pbmZyYS1ob3N0LmNvbS9vYXV0aC90b2tlbiIsImdyYW50X3R5cGUiOiJhdXRob3JpemF0aW9uX2NvZGUiLCJ1c2VyX25hbWUiOiJvbWFyeC5zaGFtc2VsZGluQGludGVsLmNvbSIsInVzZXJfaWQiOiI0N0I0MzU5Rjc4MTU5RUE3RTAyN0NDRTY2RTRFODk1RiIsImF1ZCI6WyJzd2Fzc2V0IiwiZGV2aWNlcHJvZmlsZSIsInVtIiwidWFtIiwiZG9jc3RvcmFnZSIsImVwcyIsInRpbWVzZXJpZXMiLCJvcGVuaWQiLCJ1YWEiLCJiaSIsImFwcHNldHRpbmdzIiwidGVtcHN0b3JhZ2UiXX0.ZOxfUxuh5eDSrFNqWCyBxOJO2NrPLgZRvub7M7Suu_0"
    #else
    static let BaseUAA2URL         = "https://sys-uaa.td.wearables.infra-host.com"
    static let BaseAppURL          = "https://sys-app.td.wearables.infra-host.com"
    static let AuthenticationToken = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiJmODYwMGNjZi1hZjNhLTRhMzYtOWUxMS03Mjk3ZTEzM2U1NjEiLCJzdWIiOiIzNjE2NDI4YS1hNjNlLTRiZjAtOGVmZi0wYjg5OTAzYzc3OTYiLCJzY29wZSI6WyJhbGwudXNlciJdLCJpYXQiOjE0NDg5MjA0MjgsImV4cCI6MTQ4MDQ3ODAyOCwiY2lkIjoibmRnVGh1bmRlckRvbWUiLCJsb2dpbl90eXBlIjoidWFhIiwiaXNzIjoiaHR0cHM6Ly9zeXMtdWFhLnRkLndlYXJhYmxlcy5pbmZyYS1ob3N0LmNvbS9vYXV0aC90b2tlbiIsImdyYW50X3R5cGUiOiJhdXRob3JpemF0aW9uX2NvZGUiLCJ1c2VyX25hbWUiOiJvbWFyeC5zaGFtc2VsZGluQGludGVsLmNvbSIsInVzZXJfaWQiOiI0N0I0MzU5Rjc4MTU5RUE3RTAyN0NDRTY2RTRFODk1RiIsImF1ZCI6WyJzd2Fzc2V0IiwiZGV2aWNlcHJvZmlsZSIsInVtIiwidWFtIiwiZG9jc3RvcmFnZSIsInRzbyIsInRpbWVzZXJpZXMiLCJleHRlbmRlZHVzZXJwcm9maWxlIiwib3BlbmlkIiwiYXBwc2V0dGluZ3MiLCJiaSIsInVhYSIsInRlbXBzdG9yYWdlIl19.K9pm1MqK9MkNe23h1bnWF53YXkt2M9DN-Gw7IpL_2xo"
    #endif
}
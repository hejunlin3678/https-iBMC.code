import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import {
    UserInfoService,
    LanguageService,
    AlertMessageService,
    ErrortipService,
    LoadingService,
    CommonService,
    SystemLockService,
    TimeoutService,
} from 'src/app/common-app/service';
import { LicenseService } from './services';
import { TiMessageService, TiTableSrcData, TiTableRowData, TiFileItem } from '@cloud/tiny3';
import { Router } from '@angular/router';
import { getMessageId, getRandomFileName } from 'src/app/common-app/utils';

@Component({
    selector: 'app-license',
    templateUrl: './license.component.html',
    styleUrls: ['../manager.component.scss', './license.component.scss']
})
export class LicenseComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private user: UserInfoService,
        private langService: LanguageService,
        private licenseService: LicenseService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private errorTip: ErrortipService,
        private commonService: CommonService,
        private router: Router,
        private loading: LoadingService,
        private lockService: SystemLockService,
        private timeoutService: TimeoutService,
    ) { }
    public isSystemLock: boolean = false;

    public esnValue: string = '';
    public isInvalid: boolean = false;
    public isInstallation: boolean = false;
    public isInstallationBtn: boolean = true;
    public isExportBtn: boolean = false;
    public isInvalidBtn: boolean = false;
    public isDelBtn: boolean = false;
    public invalidCodeVal: string = '';
    public licenseStatus: string = '';
    public leftBasicInformation = [];
    public configureUsers: boolean = this.user.privileges.indexOf('ConfigureUsers') >= 0;
    public displayed = [];
    public data: TiTableRowData[] = [];
    public srcDataFeature: TiTableSrcData;
    public licenseColums = [
        { title: this.translate.instant('LICENSE_FEATURE_NUMBER'), width: '8%' },
        { title: this.translate.instant('LICENSE_FEATURE_NAME'), width: '30%' },
        { title: this.translate.instant('LICENSE_FEATURE_STATE'), width: '20%' },
        { title: this.translate.instant('LICENSE_SALE_STATE'), width: '20%' },
        { title: this.translate.instant('LICENSE_EXPIRATION'), width: '20%' }
    ];

    public licenseModel = {
        elementId: 'license_upload',
        buttonText: this.translate.instant('LICENSE_INSTALL'),
        uploaderConfig: {
            url: '/UI/Rest/BMCSettings/LicenseService/InstallLicense',
            filters: [{
                name: 'type',
                params: ['.xml']
            }, {
                name: 'maxSize',
                params: [1024 * 1024]
            }],
            headers: {
                'From': 'WebUI',
                'X-CSRF-Token': this.user.getToken()
            },
            autoUpload: true,
            type: 'POST',
            alias: 'Content',
            onBeforeSendItems: ($event): void => {
                this.loading.state.next(true);
            },
            onCompleteItem: ($event): void => {
                if ($event.status === 200) {
                    this.init();
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                } else {
                    if (
                        JSON.parse($event.response)?.error.length > 0 &&
                        JSON.parse($event.response).error[0]?.code === 'NoValidSession'
                    ) {
                        const errorMessage = this.errorTip.getErrorMessage('NoValidSession');
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                        this.timeoutService.error401.next('NoValidSession');
                    }
                    this.loading.state.next(false);
                    const msgId = getMessageId($event.response);
                    if (msgId && msgId.length > 0) {
                        const errorId = msgId[0]?.errorId;
                        let errorMessage = 'COMMON_FAILED';
                        if (errorId) {
                            errorMessage = this.errorTip.getErrorMessage(errorId) || errorMessage;
                        }
                        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
                    } else {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                    }
                }
            },
            onAddItemFailed: (data: TiFileItem): void => {
                const validResults = data['validResults'];
                const validResultValue = validResults[0];
                if (validResultValue === 'maxSize') {
                    this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_UPLOAD_FILE_MORN_1M_ERROR' });
                } else if (validResultValue === 'type') {
                    this.alert.eventEmit.emit({ type: 'error', label: 'FILE_FORMAT_ERROR' });
                }
            }
        }
    };

    public lockStateChange(state) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }

    public goToSecurityConfiguration(): void {
        this.router.navigate(['/navigate/user/security']);
    }

    // init
    public init() {
        this.licenseService.findLicenseInfo().subscribe((resp: any) => {
            const data = resp['body'];
            this.isInvalid = false;
            this.esnValue = data.DeviceESN ? data.DeviceESN : '- -';
            // 失效码（revoketicket）（不为空才显示）
            this.invalidCodeVal = data.RevokeTicket;
            this.headInit(data);
            const licenseInfo = data.LicenseInfo;
            // 基本信息
            const generalInfo = licenseInfo.GeneralInfo;
            this.leftBasicInformation = [];
            this.leftBasicInformation.push({
                label: this.translate.instant('LICENSE_SN'),
                value: generalInfo.LSN
            });
            let licenseType = generalInfo.LicenseType;
            if (licenseType === 'Commercial') {
                licenseType = this.translate.instant('LICENSE_TYPE_COMMERCIAL');
            } else if (licenseType === 'Demo') {
                licenseType = this.translate.instant('LICENSE_STATUS_COMMISSION');
            } else if (licenseType === 'Trial') {
                licenseType = this.translate.instant('LICENSE_TYPE_TRY');
            } else {
                licenseType = this.translate.instant('LICENSE_STATUS_UNKNOWN');
            }
            this.leftBasicInformation.push({
                label: this.translate.instant('LICENSE_TYPE'),
                value: licenseType
            });

            let licenseClass = data.LicenseClass;
            if (licenseClass === 'Advanced') {
                licenseClass = this.translate.instant('LICENSE_LEVEL_TIP');
            }
            this.leftBasicInformation.push({
                label: this.translate.instant('LICENSE_LEVEL'),
                value: licenseClass
            });

            /**
             * 新增逻辑，先搜索SaleInfo，如果validDate为空字符串，则继续搜索KeyInfo
             * 直到找到第一个有效的截止日期位置，否则就显示空,许可证截止日期可能有多个有效值，但都是相同的，因此只需要获取第一个有效的即可
             */
            let validDate = '';
            // 先查KeyInfo,如果查不到有效值再查 SaleInfo.
            if (licenseInfo.KeyInfo) {
                const featureKey = licenseInfo.KeyInfo.FeatureKey ? licenseInfo.KeyInfo.FeatureKey : [];
                const tempArr = [];
                featureKey.forEach((item) => {
                    if (item.ValidDate !== '' && validDate === '') {
                        validDate = item.ValidDate;
                    }
                    let state = '';
                    switch (item.State) {
                        case 'N/A':
                            state = '--';
                            break;
                        case 'Normal':
                            state = this.translate.instant('LICENSE_STATUS_NORMAL');
                            break;
                        case 'Commissioning':
                            state = this.translate.instant('LICENSE_STATUS_COMMISSION');
                            break;
                        case 'Default':
                            state = this.translate.instant('LICENSE_STATUS_DEFAULT');
                            break;
                        case 'GracePeriod':
                            const remainGraceDay = data.AlarmInfo.RemainGraceDay;
                            state = this.translate.instant('LICENSE_STATUS_GRACE') + this.translate.instant('LICENSE_DAY')
                            + remainGraceDay + this.translate.instant('LICENSE_DAY01');
                            break;
                        case 'Emergency':
                            state = this.translate.instant('LICENSE_STATUS_EMERGENCY');
                            break;
                        default:
                            state = '--';
                    }

                    const temp = {};
                    temp['name'] = item.Name;
                    temp['state'] = state;
                    temp['value'] = item.Value;
                    const _validate = item.ValidDate === 'PERMANENT' ? this.translate.instant('LICENSE_PERMANENT') : item.ValidDate;
                    temp['validDate'] = _validate;
                    tempArr.push(temp);
                });
                this.srcDataFeature.data = tempArr;

                // 如查不到有效值，则查SaleInfo
                if (validDate === ''
                    && licenseInfo.SaleInfo
                    && licenseInfo.SaleInfo.OfferingProduct
                    && licenseInfo.SaleInfo.OfferingProduct.SaleItem
                ) {
                    let flag = false;
                    for (const item of licenseInfo.SaleInfo.OfferingProduct.SaleItem) {
                        const featureKeys = item.FeatureKey;
                        for (const subItem of featureKeys) {
                            if (subItem.ValidDate !== '') {
                                validDate = subItem.ValidDate;
                                flag = true;
                                break;
                            }
                        }
                        if (flag) {
                            break;
                        }
                    }

                }

                if (validDate === 'PERMANENT') {
                    validDate = this.translate.instant('LICENSE_PERMANENT');
                }
                this.leftBasicInformation.push({
                    label: this.translate.instant('LICENSE_EXPIRATION'),
                    value: validDate
                });
            }
            this.loading.state.next(false);
        });
    }
    public headInit(data) {
        const installedStatus = data.InstalledStatus;
        const fileState = data.AlarmInfo.FileState;
        const licenseStatus = data.LicenseStatus;
        if ((installedStatus === 'NotInstalled') && (fileState !== 'Incorrect' || fileState !== 'Normal')) {
            this.isInstallation = false;
        } else {
            this.isInstallation = true;
        }
        if ((installedStatus === 'NotInstalled') && (fileState === 'Incorrect')) {
            this.isInstallationBtn = true;
            this.isDelBtn = true;
        } else {
            this.isExportBtn = false;
            this.isInvalidBtn = false;
        }

        if ((installedStatus === 'Installed') && (fileState === 'Normal')) {
            this.isInstallationBtn = true;
            this.isExportBtn = true;
            this.isDelBtn = true;
            if (licenseStatus === 'Normal' || licenseStatus === 'Commissioning') {
                this.isInvalidBtn = true;
            }
        }
        /*
         * 获取许可证的状态
         * 当状态为调测（Commissioning）/宽限（GracePeriod）时，需要显示剩余的天
         */
        switch (licenseStatus) {
            case 'N/A':
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_UNKNOWN');
                break;
            case 'Default':
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_DEFAULT');
                break;
            case 'Normal':
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_NORMAL');
                break;
            case 'Commissioning':
                const remainCommissioningDay = data.AlarmInfo.RemainCommissioningDay;
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_COMMISSION')
                    + this.translate.instant('LICENSE_DAY') + remainCommissioningDay + this.translate.instant('LICENSE_DAY01');
                break;
            case 'GracePeriod':
                const remainGraceDay = data.AlarmInfo.RemainGraceDay;
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_GRACE')
                    + this.translate.instant('LICENSE_DAY') + remainGraceDay + this.translate.instant('LICENSE_DAY01');
                break;
            case 'Emergency':
                this.licenseStatus = this.translate.instant('LICENSE_STATUS_EMERGENCY');
                break;
        }
        // 失效码
        const revokeTicket = data.RevokeTicket;
        if (revokeTicket !== '') {
            this.isInvalid = true;
        }

    }

    // 下载
    public downloadFile() {
        const fileName = getRandomFileName('license.xml');
        this.licenseService.download({}).subscribe((res: any) => {
            const response = res.body;
            this.preProgress(response.url);
        });
    }

    // 查询taskprepare进度
    public preProgress(url) {
        this.licenseService.getTask(url).subscribe((resp: any) => {
            const res = resp['body'];
            if (res.prepare_progress === 100 && res.downloadurl) {
                this.commonService.restDownloadFile(res.downloadurl);
            } else if (res.prepare_progress !== 100 || !res.downloadurl) {
                setTimeout(() => {
                    this.preProgress(url);
                }, 3000);
            }
        });
    }
    // 失效
    public licenseInvalid() {
        const INSTANCE = this.tiMessage.open({
            id: 'licenseInvalidModal',
            type: 'prompt',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_YES'),
                autofocus: false,
                click: () => {
                    INSTANCE.close();
                    const queryParams = {};
                    this.loading.state.next(true);
                    this.licenseService.licenseInvalid(queryParams).subscribe((resp: any) => {
                        this.init();
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    }, (err) => { this.alert.eventEmit.emit({ type: 'error', label: 'FILE_FORMAT_ERROR' }); });
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_NO'),
                disabled: false,
                autofocus: false,
                click: () => {
                    INSTANCE.dismiss();
                }
            },
            title: this.translate.instant('LICENSE_REVOKE'),
            content: this.translate.instant('LICENSE_REVOKE_LICENSE') + '<br>' + this.translate.instant('LICENSE_NOT_RESTORED')
        });
    }

    // 删除
    public licenseDel() {
        const INSTANCE = this.tiMessage.open({
            id: 'licenseDelModal',
            type: 'prompt',
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_YES'),
                autofocus: false,
                click: () => {
                    INSTANCE.close();
                    const queryParams = {};
                    this.loading.state.next(true);
                    this.licenseService.licenseDel(queryParams).subscribe((resp: any) => {
                        this.init();
                        this.loading.state.next(false);
                        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    }, (err) => { this.alert.eventEmit.emit({ type: 'error', label: 'FILE_FORMAT_ERROR' }); });
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_NO'),
                disabled: false,
                autofocus: false,
                click: () => {
                    INSTANCE.dismiss();
                }
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content: this.translate.instant('LICENSE_DELETE_LICENSE') + '<br>' + this.translate.instant('LICENSE_RESTORED_DELETED')
        });
    }

    ngOnInit(): void {
        this.srcDataFeature = {
            data: this.data,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };

        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
        });

        this.init();
    }

}

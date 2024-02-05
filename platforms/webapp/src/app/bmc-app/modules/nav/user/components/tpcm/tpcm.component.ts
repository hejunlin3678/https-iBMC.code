import { Component, OnInit } from '@angular/core';
import { UntypedFormBuilder } from '@angular/forms';
import { TiModalRef, TiModalService, TiTableRowData, TiTableSrcData, TiValidationConfig } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { Observable } from 'rxjs';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { SecondAuthorityFail } from 'src/app/common-app/modules/user/dataType/data.model';
import { AlertMessageService, LoadingService, UserInfoService } from 'src/app/common-app/service';
import { SerAddrValidators } from 'src/app/common-app/utils/valid';
import { TpcmService } from './service';

@Component({
    selector: 'app-tpcm',
    templateUrl: './tpcm.component.html',
    styleUrls: ['./tpcm.component.scss']
})
export class TpcmComponent implements OnInit {
    constructor(
        private loading: LoadingService,
        private tpcmService: TpcmService,
        private user: UserInfoService,
        private tiModal: TiModalService,
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private alert: AlertMessageService
    ) {
        // 有管理员权限
        this.hasConfigUsers = this.user.hasPrivileges(['ConfigureUsers']);
    }
    public hasConfigUsers: boolean = false;
    public displayed = [];
    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };
    public colsNumber = 2;
    public colsGap = ['300px'];
    public labelWidth = '150px';
    private data: TiTableRowData[] = [];
    public basicResInfoData: object = {};
    public buttonSave: boolean = true;
    public noPwd: boolean = this.user.loginWithoutPassword;
    public tpcmBasicInfo = [
        {
            label: 'TPCM_PRODUCT_NAME',
            id: 'ProductName',
            content: ''
        },
        {
            label: 'TPCM_PRODUCT_MODEL',
            id: 'ProductModel',
            content: ''
        },
        {
            label: 'TPCM_PRODUCT_VERSION',
            id: 'ProductVersion',
            content: ''
        },
        {
            label: 'TPCM_MANUFACTURER',
            id: 'ProductManufacturer',
            content: ''
        },
        {
            label: 'TPCM_TCM_VENDOR',
            id: 'TCMManufacturer',
            content: ''
        },
        {
            label: 'TPCM_TCM_VERSION',
            id: 'TCMVersion',
            content: ''
        }
    ];
    // 表头数据
    public columns = [
        {
            title: 'TPCM_MEASURED_TARGET',
            width: '15%'
        },
        {
            title: 'TPCM_VERSION',
            width: '15%'
        },
        {
            title: 'TPCM_PLATFORM_STATUS_REGISTER',
            width: '15%'
        },
        {
            title: 'TPCM_ACTUAL_MEASUREMENT_VALUE',
            width: '25%'
        },
        {
            title: 'TPCM_BASE_VALUE',
            width: '15%'
        },
        {
            title: 'TPCM_CONSISTENCY',
            width: '10%'
        },
        {
            title: 'TPCM_BASE_VALUE_UPDATE',
            width: '15%'
        }
    ];
    public isTrusted: boolean = false;
    public isSystemLock: boolean = this.user.systemLockDownEnabled;
    public dialogBox = null;
    public generateData = {};
    public ipAddressPreData = '';
    public trustContent = {
        trustedUrl: 'assets/common-assets/image/tpcm/service-closed-tpcm.svg',
        isPass: false
    };
    public measureLabel = this.translate.instant('TPCM_TRUST_MEASURE_SWITCH');
    public controlLabel = this.translate.instant('TPCM_TRUST_CONTROL_SWITCH');
    public isStart: boolean = false;
    public trustText = 'TPCM_TRUST_STATE';
    public trustResultText = 'TPCM_TRUST_LEVEL_HIGH';
    public tsbEnableStatus = 0;
    public measureState: boolean = false;
    public controlState: boolean = false;
    public closeIcon: boolean = false;
    public showAlert = {
        type: 'prompt',
        openAlert: true
    };

    public tpcmConfigData: any = {
        initMeasureState: false,
        initControlState: false,
        initAlertMeasureState: false,
        initAlertControlState: false
    };
    public tpcmForm = this.fb.group({
        ipAddressControl: [this.ipAddressPreData, [SerAddrValidators.validateIp46AndDomin()]],
        enableMeasureState: [''],
        enableControlState: ['']
    });
    public baselinePreValue = '';
    public baselineValForm = this.fb.group({
        baselineValControl: [this.baselinePreValue, [SerAddrValidators.validateBaseLine()]]
    });

    public modalOptsFooter: object = [
        {
            label: this.translate.instant('COMMON_OK'),
            disabled: true
        },
        {
            label: this.translate.instant('ALARM_CANCEL'),
            disabled: false
        }
    ];
    public updateFooter: object = [
        {
            label: this.translate.instant('COMMON_SAVE'),
            disabled: false
        },
        {
            label: this.translate.instant('ALARM_CANCEL'),
            disabled: false
        }
    ];

    public tpcmParams = {};
    public param = {};

    // IPv4，IPv6，域名校验提示
    public validationIpAddress: TiValidationConfig = {
        tip: this.translate.instant('COMMON_IPV4_IPV6_DOMIN'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    // 基准值校验
    public validationBaselineVal: TiValidationConfig = {
        tip: this.translate.instant('TPCM_BASELINE_FORMAT'),
        type: 'blur',
        tipPosition: 'top-left',
        errorMessage: {
            pattern: this.translate.instant('COMMON_FORMAT_ERROR')
        }
    };
    public updateTip = 'TPCM_UPDATE_BIOS_TIP';

    // tpcm获取基本信息
    public getTpcmbasicData() {
        this.loading.state.next(true);
        this.tpcmService.getTPCMBasicInfo().subscribe(
            (res: any) => {
                this.basicResInfoData = res['body'];
                this.tpcmBasicInfo.forEach((item) => {
                    if (item.id === 'ProductName') {
                        item.content = this.translate.instant('TPCM_PRODUCT_NAME_VALUE');
                    } else if (item.id === 'TCMManufacturer') {
                        item.content = this.translate.instant('TPCM_TCM_VENDOR_VALUE');
                    } else {
                        item.content = this.basicResInfoData[item.id];
                    }
                });
            },
            (error) => {
                this.loading.state.next(false);
            }
        );
    }

    // 获取tpcm详细信息
    public getTPCMDetailedInfoData() {
        this.loading.state.next(true);
        this.tpcmService.getTPCMDetailedInfo().subscribe(
            (res: any) => {
                this.loading.state.next(false);
                const response = res['body'];
                this.srcData.data = response;
                this.getTrustedStatus(this.srcData.data);
            },
            (error) => {
                this.loading.state.next(false);
            }
        );
    }

    // 获取TPCM服务信息
    public getTpcmServiceData(firstInter?: boolean) {
        this.loading.state.next(true);
        this.tpcmService.getTpcmService().subscribe(
            (res: any) => {
                this.loading.state.next(false);
                this.tsbEnableStatus = res.body.TsbEnableStatus;
                this.tpcmConfigData.initMeasureState = Boolean(res.body.MeasureEnable);
                this.tpcmConfigData.initControlState = Boolean(res.body.ControlAction);
                this.tpcmConfigData.initAlertMeasureState = Boolean(res.body.MeasureEnable);
                this.tpcmConfigData.initAlertControlState = Boolean(res.body.ControlAction);
                this.measureState = Boolean(res.body.MeasureEnable);
                this.controlState = Boolean(res.body.ControlAction);
                const response = res?.body?.TrustedCenterServerAddr;
                this.ipAddressPreData = response;
                this.tpcmForm.patchValue({ ipAddressControl: response });
                this.getTPCMDetailedInfoData();
                this.getTrustContent(this.tsbEnableStatus);
                this.tpcmParams['MeasureEnable'] = Boolean(res.body.MeasureEnable);
                this.tpcmParams['ControlAction'] = Boolean(res.body.ControlAction);
                this.tpcmParams['TrustedCenterServerAddr'] = res?.body?.TrustedCenterServerAddr;
            },
            (error) => {
                this.loading.state.next(false);
            }
        );
    }

    // 获取可信状态
    public getTrustedStatus(arr: any[]) {
        if (arr.length > 0) {
            this.isTrusted = !Boolean(arr.find((item) => !item.IsMatch));
            if (this.tsbEnableStatus && this.isTrusted && this.tpcmConfigData.initMeasureState) {
                this.trustContent.trustedUrl = 'assets/common-assets/image/tpcm/pass-tpcm.svg';
                this.trustContent.isPass = true;
                this.trustResultText = 'TPCM_TRUST_LEVEL_HIGH';
            } else if (this.tsbEnableStatus && !this.isTrusted && this.tpcmConfigData.initMeasureState) {
                this.trustContent.trustedUrl = 'assets/common-assets/image/tpcm/not-pass-tpcm.svg';
                this.trustContent.isPass = false;
                this.trustResultText = 'TPCM_TRUST_LEVEL_LOW';
            } else if (this.tsbEnableStatus && !this.tpcmConfigData.initMeasureState) {
                this.trustContent.trustedUrl = 'assets/common-assets/image/tpcm/service-closed-tpcm.svg';
            }
        }
    }

    public generateDetailData(): void {
        this.getBtnState();
    }

    public generateBaselineData(): void {
        if (!this.baselineValForm.valid || !this.baselineValForm.root.get('baselineValControl').value) {
            this.updateFooter[0].disabled = true;
        } else {
            this.updateFooter[0].disabled = false;
        }
    }

    public showHalfModal(content) {
        this.dialogBox = this.tiModal.open(content, {
            id: 'configBox',
            closeIcon: true,
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                // reason, true:点击ok， false:点击取消
                if (!reason) {
                    modalRef.destroy(reason);
                    this.tpcmForm.patchValue({ ipAddressControl: this.ipAddressPreData });
                    this.measureState = this.tpcmConfigData.initMeasureState;
                    this.controlState = this.tpcmConfigData.initControlState;
                    this.tpcmConfigData.initAlertMeasureState = this.measureState;
                    this.tpcmConfigData.initAlertControlState = this.controlState;
                    return;
                }
                if (!this.noPwd) {
                    this.openModal('configContent', null, modalRef);
                } else {
                    this.saveTpcmConfig().subscribe(() => {
                        modalRef.destroy(true);
                    });
                }
            }
        });
    }

    public showUpdateBox(item: any, content: any) {
        if (!item.ActualDigest) {
            this.updateFooter[0].disabled = true;
        }
        // 系统锁定和非管理员用户不可刷新
        if (this.isSystemLock || !this.hasConfigUsers) {
            return;
        }
        if (item.MeasuredObject === 'BIOS') {
            this.updateTip = 'TPCM_UPDATE_BIOS_TIP';
        } else {
            this.updateTip = 'TPCM_UPDATE_OTHER_TIP';
        }
        this.baselinePreValue = item.ActualDigest;
        this.baselineValForm.patchValue({ baselineValControl: this.baselinePreValue });
        this.dialogBox = this.tiModal.open(content, {
            id: 'configBox',
            context: {
                close: (): void => {
                    if (this.noPwd) {
                        this.saveTpcmUpdate({ measuredObject: item.MeasuredObject }).subscribe();
                    } else {
                        this.openModal('updateContent', item.MeasuredObject);
                    }
                    this.dialogBox.close();
                },
                dismiss: (): void => {
                    this.dialogBox.close();
                    this.baselinePreValue = item.ActualDigest;
                    this.updateFooter[0].disabled = false;
                }
            }
        });
    }

    // 打开二次密码认证框
    private openModal(targetModal: string, measuredObject?: string, refBox?: TiModalRef): void {
        this.tiModal.open(SecondPasswordComponent, {
            id: 'myModal',
            modalClass: 'secondModal',
            context: {
                password: '',
                state: false,
                active: false
            },
            beforeClose: (modalRef: TiModalRef, reason: boolean): void => {
                const preObj = {
                    measure: this.tpcmForm.root.get('enableMeasureState').value,
                    control: this.tpcmForm.root.get('enableControlState').value,
                    address: this.tpcmForm.root.get('ipAddressControl').value
                };
                // reason, true:点击ok， false:点击取消
                if (!reason) {
                    modalRef.destroy(reason);
                    this.measureState = preObj.measure;
                    this.controlState = preObj.control;
                    this.tpcmForm.patchValue({ ipAddressControl: preObj.address });
                    return;
                }
                if (modalRef.content.instance.password === '' || !modalRef.content.instance.active) {
                    return;
                }
                modalRef.content.instance.active = false;
                // 配置内容
                if (targetModal === 'configContent') {
                    this.saveTpcmConfig(modalRef.content.instance.password).subscribe(
                        () => {
                            modalRef.destroy(reason);
                            refBox?.destroy(true);
                        },
                        (error) => {
                            // 发生错误时，清除二次认证密码
                            modalRef.content.instance.password = '';
                            const errorId = error.error.error[0].code;
                            if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                                modalRef.content.instance.state = true;
                            } else {
                                modalRef.destroy(false);
                            }
                        }
                    );
                }
                // 配置升级
                if (targetModal === 'updateContent') {
                    const password = modalRef.content.instance.password;
                    this.saveTpcmUpdate({ measuredObject, password }).subscribe(
                        () => {
                            modalRef.destroy(reason);
                        },
                        (error) => {
                            // 发生错误时，清除二次认证密码
                            modalRef.content.instance.password = '';
                            const errorId = error.error.error[0].code;
                            if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                                modalRef.content.instance.state = true;
                            } else {
                                modalRef.destroy(false);
                            }
                        }
                    );
                }
            },
            dismiss: (): void => {
                this.measureState = this.tpcmConfigData.initMeasureState;
                this.controlState = this.tpcmConfigData.initControlState;
            }
        });
    }
    private saveTpcmConfig(secPwd?: string): Observable<any> {
        // 若是非免密登录，需要添加二次认证参数
        this.param = {};
        if (!this.noPwd) {
            this.param['ReauthKey'] = window.btoa(secPwd);
        }

        if (this.tpcmParams['MeasureEnable'] !== this.measureState) {
            this.param['MeasureEnable'] = this.measureState;
        }
        if (this.tpcmParams['ControlAction'] !== this.controlState) {
            this.param['ControlAction'] = this.controlState;
        }
        if (this.tpcmParams['TrustedCenterServerAddr'] !== this.tpcmForm.root.get('ipAddressControl').value) {
            this.param['TrustedCenterServerAddr'] = this.tpcmForm.root.get('ipAddressControl').value;
        }
        // 调用方法
        return new Observable((observe) => {
            this.tpcmService.setTpcmService(this.param).subscribe({
                next: (res) => {
                    this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
                    this.getTpcmServiceData();
                    observe.next(res);
                },
                error: (error) => {
                    observe.error(error);
                }
            });
        });
    }

    private saveTpcmUpdate(params: { [key: string]: any }): Observable<any> {
        this.param = {};
        if (!this.noPwd) {
            this.param['ReauthKey'] = window.btoa(params.password);
        }
        return new Observable((observe) => {
            const successText =
                params.measuredObject === 'BIOS' ? 'TPCM_UPDATE_BIOS_SUCCESS' : 'TPCM_UPDATE_OTHER_SUCCESS';
            this.getFirmwareParameter(params.measuredObject);
            // 调用刷新的方法
            this.tpcmService.updateService(this.param).subscribe({
                next: (res) => {
                    this.alert.eventEmit.emit({ type: 'success', label: successText });
                    this.getTpcmServiceData(true);
                    observe.next(res);
                },
                error: (error) => {
                    // 发生错误时，清除二次认证密码
                    observe.error(error);
                }
            });
        });
    }
    public getTrustContent(tsbEnableStatus: number): void {
        // 未开启
        if (!tsbEnableStatus) {
            this.trustText = 'TPCM_TRUST_SERVICE_NOSTART';
            this.trustContent.trustedUrl = 'assets/common-assets/image/tpcm/service-closed-tpcm.svg';
            this.isStart = false;
        } else if (tsbEnableStatus && !this.tpcmConfigData.initMeasureState) {
            this.trustText = 'TPCM_TRUST_MESURE_OFF';
            this.trustContent.trustedUrl = 'assets/common-assets/image/tpcm/service-closed-tpcm.svg';
            this.isStart = false;
        } else {
            // 开启
            this.trustText = 'TPCM_TRUST_STATE';
            this.isStart = true;
        }
    }

    public switchChange() {
        this.getBtnState();
    }

    public getBtnState() {
        const currentIpAddress = this.tpcmForm.root.get('ipAddressControl').value;
        // 初始值跟当前设置的值不一样是才打开弹窗
        if (
            this.tpcmConfigData.initControlState === this.controlState &&
            this.tpcmConfigData.initMeasureState === this.measureState &&
            this.ipAddressPreData === currentIpAddress
        ) {
            this.modalOptsFooter[0].disabled = true;
            return;
        }
        if (!this.tpcmForm.valid || !currentIpAddress) {
            this.modalOptsFooter[0].disabled = true;
            return;
        } else {
            this.modalOptsFooter[0].disabled = false;
        }
    }

    public init() {
        this.getTpcmbasicData();
        this.getTpcmServiceData(true);
    }

    public getFirmwareParameter(item: string) {
        const resultArr = [
            {
                name: 'BIOS',
                paramKey: 'BiosStandardDigest'
            },
            {
                name: 'SHIM',
                paramKey: 'ShimStandardDigest'
            },
            {
                name: 'GRUB',
                paramKey: 'GrubStandardDigest'
            },
            {
                name: 'grub.cfg',
                paramKey: 'GrubCfgStandardDigest'
            },
            {
                name: 'kernel',
                paramKey: 'KernelStandardDigest'
            },
            {
                name: 'initrd',
                paramKey: 'InitrdStandardDigest'
            }
        ];
        resultArr.forEach((ele, i) => {
            if (ele.name === item) {
                this.param[ele.paramKey] = this.baselineValForm.root.get('baselineValControl').value;
            }
        });
    }

    ngOnInit(): void {
        this.init();
    }
}

import { Component, OnInit, Input, OnChanges, EventEmitter, Output } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { UntypedFormBuilder } from '@angular/forms';
import { NetworkService } from '../../services';
import { AlertMessageService } from 'src/app/common-app/service';
import { TiValidators, TiValidationConfig } from '@cloud/tiny3';
import { UserInfoService } from 'src/app/common-app/service';
import { getMessageId, formatEntry } from 'src/app/common-app/utils';
import { ErrortipService } from 'src/app/common-app/service';
import { SystemLockService } from 'src/app/common-app/service';

@Component({
    selector: 'app-network-lldp',
    templateUrl: './network-lldp.component.html',
    styleUrls: ['../../network.component.scss', './network-lldp.component.scss']
})
export class NetworkLldpComponent implements OnInit, OnChanges {

    constructor(
        private translate: TranslateService,
        private fb: UntypedFormBuilder,
        private user: UserInfoService,
        private networkService: NetworkService,
        private errorTipService: ErrortipService,
        private alert: AlertMessageService,
        private lockService: SystemLockService
    ) { }

    @Input() networkLldp;
    @Input() lldpBeingEdited;
    @Input() lldpEditDisabled;
    @Input() HMMID;
    @Output() private outer = new EventEmitter();

    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;
    public lldpEnabledText: string;


    public lldpWorkModel = {
        value: this.translate.instant('IBMC_LLDP_TX_1')
    };
    public lldpDisabled: boolean = true;
    public lldpResData = {};
    public lldpParams = {};
    public param = {};

    public netLldpForm = this.fb.group({
        lldpEnableState: [{ value: null, disabled: false }, []],
        sendControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(1, 8192)]
        ],
        preiodControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(5, 32768)]
        ],
        mutiplyControl: [
            { value: null, disabled: false },
            [TiValidators.required, TiValidators.number, TiValidators.integer, TiValidators.rangeValue(2, 10)]
        ]
    });

    // 发送延迟
    public validationSend: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [1, 8192]),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('COMMON_FORMAT_ERROR'),
            number: this.translate.instant('VALID_NUMBER_INFO'),
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [1, 8192])
        }
    };
    // 发送周期
    public validationPreiod: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [5, 32768]),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('COMMON_FORMAT_ERROR'),
            number: this.translate.instant('VALID_NUMBER_INFO'),
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [5, 32768])
        }
    };
    // 邻居节点时间
    public validationMutiply: TiValidationConfig = {
        tip: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [2, 10]),
        type: 'blur',
        errorMessage: {
            required: this.translate.instant('COMMON_FORMAT_ERROR'),
            number: this.translate.instant('VALID_NUMBER_INFO'),
            integer: this.translate.instant('VALID_INTEGER_INFO'),
            rangeValue: formatEntry(this.translate.instant('VALID_VALUE_RANGE_TIP'), [2, 10])
        }
    };

    public lldpInit = (networkLldp) => {
        const valueList = {
            Tx: this.translate.instant('IBMC_LLDP_TX_1'),
            RX: this.translate.instant('IBMC_LLDP_TX_2'),
            TxRx: this.translate.instant('IBMC_LLDP_TX_3'),
            Disabled: this.translate.instant('IBMC_LLDP_TX_0')
        };
        this.lldpEnabledText = networkLldp.getLldpEnabled() ? 'FDM_ENABLED' : 'FDM_DISABLED';
        // lldp使能
        this.lldpParams['lldpEnable'] = networkLldp.getLldpEnabled();
        // 工作模式
        this.lldpParams['lldpWork'] = valueList[networkLldp.getLldpWorkMode()];
        this.lldpWorkModel.value = this.lldpParams['lldpWork'];
        // 发送延迟
        this.lldpParams['lldpSend'] = networkLldp.getTxDelaySeconds();
        // 发送周期
        this.lldpParams['lldpPreiod'] = networkLldp.getTxIntervalSeconds();
        // 邻居节点时间
        this.lldpParams['lldpMutiply'] = networkLldp.getTxHold();
        this.netLldpForm.patchValue({
            lldpEnableState: this.lldpParams['lldpEnable'],
            sendControl: this.lldpParams['lldpSend'],
            preiodControl: this.lldpParams['lldpPreiod'],
            mutiplyControl: this.lldpParams['lldpMutiply']
        });
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            if (this.isSystemLock !== res) {
                this.isSystemLock = res;
            }
        });
        if (this.isConfigureComponentsUser && !this.isSystemLock) {
            this.netLldpForm.root.get('lldpEnableState').enable();
            this.netLldpForm.root.get('sendControl').enable();
            this.netLldpForm.root.get('preiodControl').enable();
            this.netLldpForm.root.get('mutiplyControl').enable();
        } else {
            this.netLldpForm.root.get('lldpEnableState').disable();
            this.netLldpForm.root.get('sendControl').disable();
            this.netLldpForm.root.get('preiodControl').disable();
            this.netLldpForm.root.get('mutiplyControl').disable();
        }
    }
    ngOnInit() {
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.networkLldp && changes.networkLldp.currentValue) {
            this.lldpInit(changes.networkLldp.currentValue);
        }
        if (changes.HMMID) {
            this.HMMID = changes.HMMID.currentValue;
        }
    }

    // 数据是否发生变化
    public protocolEnabledEN = () => {
        if (!this.netLldpForm.valid) {
            this.lldpDisabled = true;
            return;
        }
        this.param = {};
        if (this.netLldpForm.root.get('lldpEnableState').value !== this.lldpParams['lldpEnable']) {
            this.param['LldpEnabled'] = this.netLldpForm.root.get('lldpEnableState').value;
        }
        if (parseInt(this.netLldpForm.root.get('sendControl').value, 10) !== this.lldpParams['lldpSend']) {
            this.param['TxDelaySeconds'] = parseInt(this.netLldpForm.root.get('sendControl').value, 10);
        }
        if (parseInt(this.netLldpForm.root.get('preiodControl').value, 10) !== this.lldpParams['lldpPreiod']) {
            this.param['TxIntervalSeconds'] = parseInt(this.netLldpForm.root.get('preiodControl').value, 10);
        }
        if (parseInt(this.netLldpForm.root.get('mutiplyControl').value, 10) !== this.lldpParams['lldpMutiply']) {
            this.param['TxHold'] = parseInt(this.netLldpForm.root.get('mutiplyControl').value, 10);
        }
        if (JSON.stringify(this.param) !== '{}') {
            this.lldpDisabled = false;
        } else {
            this.lldpDisabled = true;
        }
    }

    // change事件
    public lldpEnableIdChange = () => {
        this.protocolEnabledEN();
    }

    public sendChange = () => {
        this.protocolEnabledEN();
    }

    public preiodChange = () => {
        this.protocolEnabledEN();
    }

    public mutiplyChange = () => {
        this.protocolEnabledEN();
    }

    // 保存
    public lldpButton(): void {
        this.lldpDisabled = true;
        this.protocolEnabledEN();
        this.networkService.saveLldp(this.HMMID, JSON.stringify(this.param)).subscribe((response) => {
            this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
            this.lldpDisabled = true;
            this.networkService.lldpDataDetails(this.networkLldp, response['body']);
            this.lldpInit(this.networkLldp);
            this.outer.emit(false);
        }, (error) => {
            const errorId = getMessageId(error.error)[0].errorId;
            const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
            this.alert.eventEmit.emit({type: 'error', label: errorMessage});
            this.lldpInit(this.networkLldp);
            this.outer.emit(false);
        });
    }
    public lldpEdit() {
        this.outer.emit(true);
    }
    public lldpEditCancel() {
        this.netLldpForm.patchValue({
            lldpEnableState: this.lldpParams['lldpEnable'],
            sendControl: this.lldpParams['lldpSend'],
            preiodControl: this.lldpParams['lldpPreiod'],
            mutiplyControl: this.lldpParams['lldpMutiply']
        });
        if (this.netLldpForm.valid) {
            this.outer.emit(false);
        }
    }
    public lldpEditSave() {
        this.lldpButton();
    }
}

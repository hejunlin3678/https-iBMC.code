import { Component, OnInit, Input, OnChanges } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { NetworkService } from '../../services';
import { AlertMessageService, UserInfoService, ErrortipService } from 'src/app/common-app/service';
import { TiMessageService } from '@cloud/tiny3';
import { getBrowserType, getMessageId } from 'src/app/common-app/utils';

@Component({
    selector: 'app-network-port',
    templateUrl: './network-port.component.html',
    styleUrls: ['../../network.component.scss', './network-port.component.scss']
})
export class NetworkPortComponent implements OnInit, OnChanges {

    constructor(
        private translate: TranslateService,
        private networkService: NetworkService,
        private user: UserInfoService,
        private alert: AlertMessageService,
        private tiMessage: TiMessageService,
        private errorTipService: ErrortipService
    ) { }

    @Input() networkPort;
    @Input() isSystemLock;

    public browser: string = getBrowserType().browser;
    // 权限判断
    public isConfigureComponentsUser = this.user.hasPrivileges(['ConfigureComponents']);

    public modeDisabled: boolean = true;
    public schemaRadio = {
        option: [{
            id: 'Fixed',
            key: this.translate.instant('IBMC_SETTING01'),
            tip: this.translate.instant('IBMC_SELECT_MODE_TIPS1'),
            disable: false
        },
        {
            id: 'Automatic',
            key: this.translate.instant('IBMC_SETTING02'),
            tip: this.translate.instant('IBMC_SELECT_MODE_TIPS2'),
            disable: false
        }],
        value: ''
    };

    public ncsiRadio = {
        option: [{
            id: 'Auto Failover Mode',
            key: this.translate.instant('IBMC_NCSI_SETTING01'),
            disable: false
        },
        {
            id: 'Manual Switch Mode',
            key: this.translate.instant('IBMC_NCSI_SETTING02'),
            disable: false
        }],
        value: ''
    };

    public baseNetPort = {
        Type: null,
        PortNumber: null
    };
    public backDedicatedCheckbox = [];
    public backAggregationCheckbox = [];
    public backLomCheckbox = [];
    public backLom2Checkbox = [];
    public backPcieCheckbox = [];
    public backOcpCheckbox = [];
    public backOcpCheckbox2 = [];
    public networkConfigObj = {
        // 专用网口
        dedicatedInternetAc: {
            option: [],
            value: '',
            disabledNum: null
        },
        dedicatedCheckBox: {
            checkList: []
        },
        onboardInternetAc: {
            option: [],
            value: '',
            disabledNum: null
        },
        onboardCheckBox: {
            checkList: []
        },
        extendInternetAc: {
            option: [],
            value: '',
            disabledNum: null
        },
        extendCheckBox: {
            checkList: []
        },
        extendInternetAc2: {
            option: [],
            value: '',
            disabledNum: null
        },
        extendCheckBox2: {
            checkList: []
        },
        pcieInternetAc: {
            option: [],
            value: '',
            disabledNum: null
        },
        pcieCheckBox: {
            checkList: []
        },
        ocpInternetAc: {
            option: [],
            value: '',
            disabledNum: null
        },
        ocpCheckBox: {
            checkList: []
        },
        ocpInternetAc2: {
            option: [],
            value: '',
            disabledNum: null
        },
        ocpCheckBox2: {
            checkList: []
        }
    };
    public networkPortType = {
        Type: null,
        PortNumber: null,

    };
    public queryParams = {
        NetworkPort: {
            ManagementNetworkPort: {},
            Members: {}
        }
    };
    public portParams = {};

    public portInit = (networkPort) => {
        this.networkConfigObj.dedicatedInternetAc.value = '';
        this.networkConfigObj.onboardInternetAc.value = '';
        this.networkConfigObj.extendInternetAc.value = '';
        this.networkConfigObj.extendInternetAc2.value = '';
        this.networkConfigObj.pcieInternetAc.value = '';
        this.networkConfigObj.ocpInternetAc.value = '';
        this.networkConfigObj.ocpInternetAc2.value = '';
        if (JSON.stringify(networkPort) === '{}') {
            return;
        }
        // 选择模式
        this.portParams['schemaRadio'] = networkPort.getSchemaRadio();
        this.schemaRadio.value = this.portParams['schemaRadio'];
        this.portParams['ncsiRadio'] = networkPort.getNcsiRadio();
        this.ncsiRadio.value = this.portParams['ncsiRadio'];
        // 管理网口
        this.networkPortType = networkPort.getNetworkPortType();
        // 作为基础管理网口的数据
        this.baseNetPort = this.copy(this.networkPortType);

        // Radio 根据接口的管理网口字段，来选中固定设置具体的网口和端口
        const relationRadio = {
            Dedicated: {
                origin: networkPort.getTempDedicated(),
                target: this.networkConfigObj.dedicatedInternetAc
            },
            Aggregation: {
                origin: networkPort.getTempAggregation(),
                target: this.networkConfigObj.onboardInternetAc
            },
            LOM: {
                origin: networkPort.getTempExtend(),
                target: this.networkConfigObj.extendInternetAc
            },
            FlexIO: {
                origin: networkPort.getTempExtend2(),
                target: this.networkConfigObj.extendInternetAc2
            },
            ExternalPCIe: {
                origin: networkPort.getTempPcie(),
                target: this.networkConfigObj.pcieInternetAc
            },
            OCP: {
                origin: networkPort.getTempOcp(),
                target: this.networkConfigObj.ocpInternetAc
            },
            OCP2: {
                origin: networkPort.getTempOcp2(),
                target: this.networkConfigObj.ocpInternetAc2
            }
        };
        const tarRadio = relationRadio[this.baseNetPort.Type] || {};
        if (tarRadio.origin) {
            tarRadio.origin.forEach((itemData) => {
                if (itemData.id === this.baseNetPort.PortNumber) {
                    tarRadio.target.value = itemData.id;
                }
            });
        }

        /*
        * 数据赋值与备份
        * 专用网口
        */
        this.networkConfigObj.dedicatedInternetAc.option = networkPort.getTempDedicated();
        this.networkConfigObj.dedicatedInternetAc.disabledNum = networkPort.getCountDedicated();
        this.networkConfigObj.dedicatedCheckBox.checkList = networkPort.getTempCheckDedicated();
        this.backDedicatedCheckbox = this.copyArr(networkPort.getTempCheckDedicated());
        // 汇聚网口
        this.networkConfigObj.onboardInternetAc.option = networkPort.getTempAggregation();
        this.networkConfigObj.onboardInternetAc.disabledNum = networkPort.getCountAggregation();
        this.networkConfigObj.onboardCheckBox.checkList = networkPort.getTempCheckAggregation();
        this.backAggregationCheckbox = this.copyArr(networkPort.getTempCheckAggregation());
        // 板载网口
        this.networkConfigObj.extendInternetAc.option = networkPort.getTempExtend();
        this.networkConfigObj.extendInternetAc.disabledNum = networkPort.getCountExtend();
        this.networkConfigObj.extendCheckBox.checkList = networkPort.getTempCheckExtend();
        this.backLomCheckbox = this.copyArr(networkPort.getTempCheckExtend());
        // 板载网口2
        this.networkConfigObj.extendInternetAc2.option = networkPort.getTempExtend2();
        this.networkConfigObj.extendInternetAc2.disabledNum = networkPort.getCountExtend2();
        this.networkConfigObj.extendCheckBox2.checkList = networkPort.getTempCheckExtend2();
        this.backLom2Checkbox = this.copyArr(networkPort.getTempCheckExtend2());

        // PCIE扩展网口
        this.networkConfigObj.pcieInternetAc.option = networkPort.getTempPcie();
        this.networkConfigObj.pcieInternetAc.disabledNum = networkPort.getCountPcie();
        this.networkConfigObj.pcieCheckBox.checkList = networkPort.getTempCheckPcie();
        this.backPcieCheckbox = this.copyArr(networkPort.getTempCheckPcie());
        // OCP扩展网口
        this.networkConfigObj.ocpInternetAc.option = networkPort.getTempOcp();
        this.networkConfigObj.ocpInternetAc.disabledNum = networkPort.getCountOcp();
        this.networkConfigObj.ocpCheckBox.checkList = networkPort.getTempCheckOcp();
        this.backOcpCheckbox = this.copyArr(networkPort.getTempCheckOcp());
        // OCP2扩展网口
        this.networkConfigObj.ocpInternetAc2.option = networkPort.getTempOcp2();
        this.networkConfigObj.ocpInternetAc2.disabledNum = networkPort.getCountOcp2();
        this.networkConfigObj.ocpCheckBox2.checkList = networkPort.getTempCheckOcp2();
        this.backOcpCheckbox2 = this.copyArr(networkPort.getTempCheckOcp2());
    }
    ngOnInit() {
    }

    ngOnChanges(changes: import('@angular/core').SimpleChanges): void {
        if (changes.networkPort && changes.networkPort.currentValue) {
            this.portInit(changes.networkPort.currentValue);
        }
        this.addLabelId();
    }

    public protocolEnabledEN = () => {
        this.queryParams = {
            NetworkPort: {
                ManagementNetworkPort: {},
                Members: {}
            }
        };
        if (this.schemaRadio.value === 'Fixed') {
            // 固定模式 则只能选择一种模式，那种模式被选中，则传哪个模式的参数
            const type = this.networkPortType.Type;
            let portNumber = 1;
            if (type === 'Dedicated') {
                portNumber = Number(this.networkConfigObj.dedicatedInternetAc.value);
            } else if (type === 'LOM') {
                portNumber = Number(this.networkConfigObj.extendInternetAc.value);
            } else if (type === 'FlexIO') {
                portNumber = Number(this.networkConfigObj.extendInternetAc2.value);
            } else if (type === 'Aggregation') {
                portNumber = Number(this.networkConfigObj.onboardInternetAc.value);
            } else if (type === 'ExternalPCIe') {
                portNumber = Number(this.networkConfigObj.pcieInternetAc.value);
            } else if (type === 'OCP') {
                portNumber = Number(this.networkConfigObj.ocpInternetAc.value);
            } else if (type === 'OCP2') {
                portNumber = Number(this.networkConfigObj.ocpInternetAc2.value);
            }
            this.queryParams.NetworkPort.ManagementNetworkPort = {
                Type: type,
                PortNumber: portNumber
            };
            this.queryParams.NetworkPort['Mode'] = this.schemaRadio.value;
        } else if (this.schemaRadio.value === 'Automatic') {
            // 自选模式
            // 专用网口
            const dedication = this.networkConfigObj.dedicatedCheckBox.checkList;
            // 汇聚网口
            const onboard = this.networkConfigObj.onboardCheckBox.checkList;
            // 板载网口
            const extend = this.networkConfigObj.extendCheckBox.checkList;
            // 板载网口2
            const extend2 = this.networkConfigObj.extendCheckBox2.checkList;
            // PCIE扩展网口
            const pCIE = this.networkConfigObj.pcieCheckBox.checkList;
            // OCP扩展网口
            const ocp = this.networkConfigObj.ocpCheckBox.checkList;
            // OCP2扩展网口
            const ocp2 = this.networkConfigObj.ocpCheckBox2.checkList;

            const dedicatedPort = [];
            const aggregationPort = [];
            const lomPort = [];
            const flexIOPort = [];
            const externalPCIePort = [];
            const ocpPort = [];
            const ocpPort2 = [];
            // 专用网口
            for (const key of dedication) {
                dedicatedPort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // 汇聚网口
            for (const key of onboard) {
                aggregationPort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // 板载网口
            for (const key of extend) {
                lomPort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // 板载网口2
            for (const key of extend2) {
                flexIOPort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // PCIE扩展网口
            for (const key of pCIE) {
                externalPCIePort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // OCP扩展网口
            for (const key of ocp) {
                ocpPort.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            // OCP2扩展网口
            for (const key of ocp2) {
                ocpPort2.push({
                    PortNumber: Number(key.id),
                    AdaptiveFlag: key.checked
                });
            }
            this.queryParams.NetworkPort.Members['Dedicated'] = dedicatedPort;
            this.queryParams.NetworkPort.Members['Aggregation'] = aggregationPort;
            this.queryParams.NetworkPort.Members['LOM'] = lomPort;
            this.queryParams.NetworkPort.Members['FlexIO'] = flexIOPort;
            this.queryParams.NetworkPort.Members['ExternalPCIe'] = externalPCIePort;
            this.queryParams.NetworkPort.Members['OCP'] = ocpPort;
            this.queryParams.NetworkPort.Members['OCP2'] = ocpPort2;
            this.queryParams.NetworkPort['Mode'] = this.schemaRadio.value;
            console.log(this.ncsiRadio.value);
            this.queryParams.NetworkPort['NcsiMode'] = this.ncsiRadio.value;
        }
        this.deleteNull(this.queryParams.NetworkPort);
        return this.queryParams;
    }
    // change事件
    public schemaRadioChange = () => {
        // 网口模式从手动模式切换到自动模式，NCSI模式默认选中“手动”模式
        if (this.schemaRadio.value === 'Automatic' && !this.ncsiRadio.value) {
            this.ncsiRadio.value = 'Manual Switch Mode';
        }
        this.checkSaveState();
    }
    // NCSI模式change事件
    public ncsiRadioChange = () => {
        this.checkSaveState();
    }
    // 自动选择change事件
    public selectAdaptive = () => {
        this.checkSaveState();
    }

    // 固定模式change事件
    public networkInternetChange(item) {
        this.networkConfigObj.dedicatedInternetAc.value = '';
        this.networkConfigObj.onboardInternetAc.value = '';
        this.networkConfigObj.extendInternetAc.value = '';
        this.networkConfigObj.extendInternetAc2.value = '';
        this.networkConfigObj.pcieInternetAc.value = '';
        this.networkConfigObj.ocpInternetAc.value = '';
        this.networkConfigObj.ocpInternetAc2.value = '';
        // 选中当前网口;
        if (item.Type === 'Dedicated') {
            this.networkConfigObj.dedicatedInternetAc.value = item.id;
        } else if (item.Type === 'Aggregation') {
            this.networkConfigObj.onboardInternetAc.value = item.id;
        } else if (item.Type === 'LOM') {
            this.networkConfigObj.extendInternetAc.value = item.id;
        } else if (item.Type === 'FlexIO') {
            this.networkConfigObj.extendInternetAc2.value = item.id;
        } else if (item.Type === 'ExternalPCIe') {
            this.networkConfigObj.pcieInternetAc.value = item.id;
        } else if (item.Type === 'OCP') {
            this.networkConfigObj.ocpInternetAc.value = item.id;
        } else if (item.Type === 'OCP2') {
            this.networkConfigObj.ocpInternetAc2.value = item.id;
        }
        // 当前选中的网口模式;
        this.networkPortType.Type = item.Type;
        this.networkPortType.PortNumber = item.id;
        this.checkSaveState();
    }

    // 处理按钮的灰化
    public checkSaveState() {
        let flag1 = false;
        // 首先判断网口模式的选择
        if (this.schemaRadio.value !== this.portParams['schemaRadio']) {
            flag1 = true;
        }
        // 其次，当时固定模式时，校验radioList数据是否变化，否则校验checkList值是否变化
        let flag2 = false;
        if (this.schemaRadio.value === 'Fixed') {
            // 固定模式下的对比是按照，当前选中的
            if (this.baseNetPort.Type !== this.networkPortType.Type
                || this.baseNetPort.PortNumber !== this.networkPortType.PortNumber) {
                flag2 = true;
            }
        } else {
            flag2 = this.compareArray(
                this.backDedicatedCheckbox,
                this.networkConfigObj.dedicatedCheckBox.checkList
            );
            if (!flag2) {
                flag2 = this.compareArray(
                    this.backAggregationCheckbox,
                    this.networkConfigObj.onboardCheckBox.checkList
                );
            }
            if (!flag2) {
                flag2 = this.compareArray(
                    this.backLomCheckbox,
                    this.networkConfigObj.extendCheckBox.checkList
                );
            }
            if (!flag2) {
                flag2 = this.compareArray(
                    this.backLom2Checkbox,
                    this.networkConfigObj.extendCheckBox2.checkList
                );
            }
            if (!flag2) {
                flag2 = this.compareArray(
                    // PCIE扩展网口
                    this.backPcieCheckbox,
                    this.networkConfigObj.pcieCheckBox.checkList
                );
            }
            if (!flag2) {
                flag2 = this.compareArray(
                    this.backOcpCheckbox,
                    this.networkConfigObj.ocpCheckBox.checkList);
            }
            if (!flag2) {
                flag2 = this.compareArray(
                    this.backOcpCheckbox2,
                    this.networkConfigObj.ocpCheckBox2.checkList);
            }
            if (!flag2) {
                flag2 = this.ncsiRadio.value !== this.portParams['ncsiRadio'];
            }
        }
        // 设置按钮的灰化和正常状态
        if (flag1 || flag2) {
            this.modeDisabled = false;
        } else {
            this.modeDisabled = true;
        }
        this.addLabelId();
    }

    // 比较2个数组的数据是否相同
    public compareArray(origin, target) {
        if (
            Object.prototype.toString.call(origin) !== '[object Array]'
            || Object.prototype.toString.call(target) !== '[object Array]'
        ) {
            return false;
        }
        if (origin.length === 0) {
            return false;
        }

        let flag = false;
        for (const key1 of origin) {
            for (const key2 of target) {
                if (key2.id === key1.id && key2.checked !== key1.checked) {
                    flag = true;
                    break;
                }
                if (flag) {
                    break;
                }
            }
        }
        return flag;
    }

    // 保存
    public modeButton(): void {
        this.protocolEnabledEN();
        const instance = this.tiMessage.open({
            id: 'netPort',
            type: 'prompt',
			content: this.translate.instant('NETWORK_PORT_CHANGE_TIP'),
			okButton: {
				show: true,
                text: this.translate.instant('NETWORK_PORT_CHANGE_OK'),
                autofocus: false,
				click: () => {
                    instance.close();
                    this.modeDisabled = true;
                    this.networkService.saveNetWork(JSON.stringify(this.queryParams)).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        this.modeDisabled = true;
                        this.networkService.portDetails(this.networkPort, response['body']);
                        this.portInit(this.networkPort);
                    }, (error) => {
                        const errorId = getMessageId(error.error)[0].errorId;
                        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
                        this.alert.eventEmit.emit({type: 'error', label: errorMessage});
                        this.networkService.portDetails(this.networkPort, error.error.data);
                        this.portInit(this.networkPort);
                    });
				}
			},
			cancelButton: {
				show: true,
				text: this.translate.instant('ALARM_CANCEL'),
				click: () => {
					instance.dismiss();
                    // 处理按钮的灰化
                    this.checkSaveState();
				}
			},
			title: this.translate.instant('ALARM_OK'),
        });
    }

    public copy(item) {
        return Object.assign({}, item);
    }

    public copyArr(item) {
        const arr = [];
        for (const key of item) {
            arr.push(Object.assign({}, key));
        }
        return arr;
    }
    /**
     * 过滤掉对象中的空值
     * @param obj 要过滤的对象集合
     * @returns 过滤后的对象
     */
	public deleteNull(obj: any) {
		for (const key in obj) {
			if (
				Object.prototype.hasOwnProperty.call(obj, key)
				&& (JSON.stringify(obj[key]) === '{}')
			) {
				delete obj[key];
			}
		}
		return obj;
	}
    public addLabelId() {
        // 异步操作等待节点渲染后才能获取节点
        setTimeout(() => {
            // tiny框架版本未给label添加id，为与master版本一致添加id
            const labelCheckbox = document.querySelectorAll('label.ti3-checkbox');
            const labelRadio = document.querySelectorAll('label.ti3-radio');
            labelCheckbox.forEach((item) => {
                item.setAttribute('id', item.getAttributeNode('for').value + '_checkbox');
            });
            labelRadio.forEach((item) => {
                item.setAttribute('id', item.getAttributeNode('for').value + '_radio');
            });
        }, 0);
    }
}

import { Component, OnInit, Input } from '@angular/core';
import { ICheckList } from '../../interface';
import { TranslateService } from '@ngx-translate/core';

@Component({
    selector: 'app-power-info-setting',
    templateUrl: './power-info-setting.component.html',
    styleUrls: ['./power-info-setting.component.scss']
})
export class PowerInfoSettingComponent implements OnInit {

    @Input() isActiveAndStandby: string;
    @Input() activePSU: ICheckList[];
    @Input() deepSleep: boolean;
    @Input() normalAndRedundancy: boolean;
    @Input() param;

    public saveBtn: boolean = true;

    constructor(
        private translate: TranslateService
    ) { }

    public workingMode: {label: string, tip: string, radioList: {key: string, id: string}[], disabled: boolean, selected: string} = {
        label: this.translate.instant('POWER_WORKING_MODE'),
        tip: this.translate.instant('POWER_WORKING_MODE_TIP'),
        disabled: false,
        radioList: [
            {
                key: this.translate.instant('POWER_LOAD_BALANCING'),
                id: 'Sharing'
            }, {
                key: this.translate.instant('POWER_MAIN_STANDBY'),
                id: 'Active/Standby'
            }
        ],
        selected: ''
    };

    public activePower: {label: string, lists: any[]} = {
        label: this.translate.instant('POWER_MAIN'),
        lists: []
    };

    public deepDormancy: {label: string, tip: string, switchState: boolean} = {
        label: this.translate.instant('POWER_DEEP_DORMANCY'),
        tip: this.translate.instant('POWER_OS_SHUTDOWN'),
        switchState: false
    };

    public workingModeChange() {
        if (this.workingMode.selected === 'Active/Standby') {
            this.activePower.lists.forEach(element => {
                element.disabled = false;
                element.checked = false;
            });
        } else {
            this.activePower.lists.forEach(element => {
                element.disabled = true;
                element.checked = false;
            });
        }
        this.monitorBtn();
    }

    public activePowerChange(model, item) {
        item.checked = model;
        this.monitorBtn();
    }

    public deepDormancyChange() {
        this.deepDormancy.switchState = !this.deepDormancy.switchState;
        this.monitorBtn();
    }

    private monitorBtn() {
        const param = this.getParam();
        if (param) {
            this.saveBtn = false;
        } else {
            this.saveBtn = true;
        }
    }

    private getParam() {
        const param = { };
        // 判断还有备用电源
        let isStandby = 0;
        let deepDormancy = '';
        const supplyList = [];
        if (this.workingMode.selected !== this.isActiveAndStandby) {
            param['PowerMode'] = this.workingMode.selected;
        }
        if (this.workingMode.selected === 'Active/Standby') {
            let noActive = true;
            isStandby = 1;
            this.activePower.lists.forEach(element => {
                if (element.checked) {
                    supplyList.push({
                        Name: element.name,
                        Mode: 'Active'
                    });
                    noActive = false;
                } else {
                    supplyList.push({
                        Name: element.name,
                        Mode: 'Standby'
                    });
                    isStandby = 0;
                }
            });
            if (noActive) {
                isStandby = 2;
            }
            param['SupplyList'] = supplyList;
        }
        if (this.deepDormancy.switchState !== this.deepSleep) {
            deepDormancy = this.deepDormancy.switchState ? 'On' : 'Off';
            param['DeepSleep'] = deepDormancy;
        }
        if (JSON.stringify(param) !== '{}' && !isStandby) {
            this.param = param;
        } else {
            this.param = null;
            if (JSON.stringify(param) !== '{}') {
                if (isStandby === 1) {
                    this.param = 1;
                } else if ( isStandby === 2) {
                    this.param = 2;
                }
            }
        }
        return this.param;

    }


    init() {
        this.workingMode.selected = this.isActiveAndStandby;
        this.deepDormancy.switchState = this.deepSleep;
        this.workingMode.disabled = this.normalAndRedundancy;
        this.activePSU.forEach((iterator) => {
            this.activePower.lists.push({
                label: iterator.label,
                name: iterator.name,
                checked: iterator.checked,
                disabled: this.normalAndRedundancy || iterator.disabled
            });
        });
    }

    close(): void {
    }

    dismiss(): void {
    }

    ngOnInit(): void {
        this.init();
    }

}

import { Component, ElementRef, EventEmitter, Input, OnChanges, Output, SimpleChanges, ViewChild } from '@angular/core';
import { UntypedFormBuilder, UntypedFormControl, UntypedFormGroup } from '@angular/forms';
import { TiTableColumns, TiTableRowData, TiTableSrcData, TiValidators } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
import { UserInfoService } from 'src/app/common-app/service';
import { formatEntry } from 'src/app/common-app/utils';
import { Blade } from '../../model/blade';
import { Chassis } from '../../model/chassis';

@Component({
    selector: 'app-blade-power-capping',
    templateUrl: './blade-power-capping.component.html',
    styleUrls: ['./blade-power-capping.component.scss']
})
export class BladePowerCappingComponent implements OnChanges {

    @Input() chassis: Chassis;
    @Input() isManual: string;
    @Input() capValue: number;
    @Output() private outer = new EventEmitter();
    @ViewChild('bladeTable', {static: true}) bladeTable: ElementRef;
    public bladeForm: UntypedFormGroup = this.fb.group({});
    public isPrivileges = this.user.hasPrivileges(['OemPowerControl']);
    constructor(
        private user: UserInfoService,
        private fb: UntypedFormBuilder,
        private translate: TranslateService,
    ) { }

    // 表示表格实际呈现的数据（开发者默认设置为[]即可）
    public displayed: TiTableRowData[] = [];
    public srcData: TiTableSrcData;
    public bladeList;
    public columns: TiTableColumns[] = [
        {
            title: 'INFO_MAIN_BOARD_SLOT'
        },
        {
            title: 'POWER_CAPPING_REALTIME'
        },
        {
            title: 'POWER_CAPPING_CHESSIS_VALUE',
            width: '350px'
        },
        {
            title: 'POWER_CAPPING_MANUAL_SETTING'
        },
        {
            title: 'POWER_CAPPING_FAILURE_POLICY'
        }
    ];

    // 需要在修改机箱功率封顶值和开启刀片手动模式和每一次刀片功率封顶值输入完毕时调用
    public refreshVerification() {
        // 剩余可设置功率封顶值 = 当前输入机框功率封顶值 - 机框最小功率封顶值（包含所有单板已经手动设置的功率封顶值）
        let maxValueWatts = this.capValue - this.chassis.minCapValue;
        const bladeList = this.chassis.bladeList;
        bladeList.forEach(blade => {
            const isBladeManual = this.bladeForm.controls[`switch${blade.id}`].value;
            const bladeCapValueValid = this.bladeForm.controls[`blade${blade.id}`].valid;
            const bladeCapValue = bladeCapValueValid ? Number(this.bladeForm.controls[`blade${blade.id}`].value) : 0;
            if (isBladeManual) {
                // 若当前为手动模式，则表示已配置使用的功率封顶值
                maxValueWatts -= bladeCapValue;
                if (blade.isManual === isBladeManual) {
                    // 若默认为手动模式，当前也为手动模式，则需要在加回默认值（机框最新功率封顶值已经包含了手动设置的）
                    maxValueWatts += blade.capValue;
                }
            }
        });
        this.bladeList.forEach((blade, index) => {
            const isBladeManual = this.bladeForm.controls[`switch${blade.id}`].value;
            const bladeCapValueValid = this.bladeForm.controls[`blade${blade.id}`].valid;
            const bladeCapValue = bladeCapValueValid ? Number(this.bladeForm.controls[`blade${blade.id}`].value) : 0;
            if (isBladeManual) {
                // 剩余可设置的功率封顶值 + 当前已经输入的功率封顶值 = 当前刀片可设置的最大功率封顶值
                let currentMaxValueWatts = maxValueWatts + bladeCapValue;
                currentMaxValueWatts = currentMaxValueWatts > 10000 ? 10000 : currentMaxValueWatts;
                const validatorChecked = [
                    TiValidators.required,
                    TiValidators.digits,
                    TiValidators.rangeValue(1, currentMaxValueWatts),
                ];
                this.bladeForm.controls[`blade${blade.id}`].setValidators(validatorChecked);
                this.bladeList[index].capValue.valid = {
                    tipMaxWidth: '350px',
                    tipPosition: 'top',
                    tip: formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                        [blade.recommendMinCapValue, currentMaxValueWatts]),
                    errorMessage: {
                        required:  formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                    [blade.recommendMinCapValue, currentMaxValueWatts]),
                        digits:  formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                    [blade.recommendMinCapValue, currentMaxValueWatts]),
                        rangeValue:  formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                    [blade.recommendMinCapValue, currentMaxValueWatts]),
                    }
                };
            }
        });
    }

    public getParam() {
        const bladeParam = [];
        const vaild = this.bladeForm.valid;
        if (vaild) {
            const bladeList = this.chassis.bladeList;
            bladeList.forEach(blade => {
                const param = {};
                if (this.bladeForm.controls[`blade${blade.id}`] &&
                blade.capValue !== Number(this.bladeForm.controls[`blade${blade.id}`].value)) {
                    param['capValue'] = Number(this.bladeForm.controls[`blade${blade.id}`].value);
                }
                if (this.bladeForm.controls[`switch${blade.id}`] && blade.isManual !== this.bladeForm.controls[`switch${blade.id}`].value) {
                    param['isManual'] = this.bladeForm.controls[`switch${blade.id}`].value;
                }
                if (this.bladeForm.controls[`radio${blade.id}`] &&
                blade.failurePolicy !== this.bladeForm.controls[`radio${blade.id}`].value.split(blade.id)[0]) {
                    param['failurePolicy'] = this.bladeForm.controls[`radio${blade.id}`].value.split(blade.id)[0];
                }
            });
        }
        this.outer.emit(bladeParam.length ? bladeParam : null);
    }

    public init() {
        const bladeList = this.chassis.bladeList;
        const isManual = this.chassis.capMode === 'Manual';
        this.bladeForm = this.fb.group({});
        this.bladeList = bladeList.map((item: Blade) => {
            let maxValueWatts = this.chassis.capValue - this.chassis.minCapValue;
            if (item.isManual) {
                maxValueWatts += item.capValue;
            }
            maxValueWatts = maxValueWatts > 10000 ? 10000 : maxValueWatts;
            const formControlName = `blade${item.id}`;
            const switchFormControlName = `switch${item.id}`;
            const radioFormControlName = `radio${item.id}`;
            const validatorChecked = [
                TiValidators.required,
                TiValidators.digits,
                TiValidators.rangeValue(1, maxValueWatts),
            ];
            this.bladeForm.addControl(formControlName,
                new UntypedFormControl({value: item.capValue, disabled: !this.isPrivileges || !item.isManual}, validatorChecked));
            this.bladeForm.addControl(switchFormControlName,
                new UntypedFormControl({value: item.isManual, disabled: !this.isPrivileges || !isManual}));
            this.bladeForm.addControl(radioFormControlName,
                new UntypedFormControl({value: item.failurePolicy + item.id, disabled: !this.isPrivileges}));
            const blade = {
                id: item.id,
                slot: item.id,
                actualPower: item.actualPower,
                recommendMinCapValue: item.recommendMinCapValue,
                formControlName,
                switchFormControlName,
                radioFormControlName,
                capValue: {
                    valid: {
                        tipMaxWidth: '350px',
                        tipPosition: 'top',
                        tip: formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                             [item.recommendMinCapValue, maxValueWatts]),
                        errorMessage: {
                            required: formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                      [item.recommendMinCapValue, maxValueWatts]),
                            digits: formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                      [item.recommendMinCapValue, maxValueWatts]),
                            rangeValue: formatEntry(this.translate.instant('VALID_ALLOCATABLE_VALUE'),
                                      [item.recommendMinCapValue, maxValueWatts]),
                        }
                    },
                    change: () => {
                        this.getParam();
                    },
                    blur: () => {
                        this.refreshVerification();
                    }
                },
                isManual: {
                    onNgModelChange: (row: any) => {
                        row.capValue.disabled = !row.isManual.switchState;
                        if (this.bladeForm.controls[row.switchFormControlName].value) {
                            this.bladeForm.controls[row.formControlName].enable();
                        } else {
                            this.bladeForm.controls[row.formControlName].disable();
                        }
                        this.refreshVerification();
                        this.getParam();
                        setTimeout(() => {
                            this.bladeTable.nativeElement.querySelector(`#input${row.id}`).focus();
                        }, 0);
                    }
                },
                failurePolicy: {
                    list: [{
                        id: `PowerOff${item.id}`,
                        key: 'POWER_OFF'
                    }, {
                        id: `NoAction${item.id}`,
                        key: 'POWER_CAPPING_NOCONTROL'
                    }],
                    change: () => {
                        this.getParam();
                    }
                }
            };
            return blade;
        });
        this.srcData = {
            data: this.bladeList,
            state: {
                searched: false,
                sorted: false,
                paginated: false
            }
        };
    }

    ngOnChanges(changes: SimpleChanges): void {
        if (changes.chassis && changes.chassis.currentValue) {
            this.init();
        }
        if (!this.bladeList) {
            return;
        }
        const isManual = changes.isManual && changes.isManual.currentValue;
        // 功率封顶模式非手动时，刀片功率封顶手动设置和功率封顶值都是禁用状态不可操作;
        // 功率封顶模式为手动时，刀片功率封顶手动设置可操作，刀片功率封顶手动设置开启时，功率封顶值可输入;
        this.bladeList.forEach((data: any) => {
            if (isManual && isManual === 'Manual') {
                this.bladeForm.controls[data.switchFormControlName].enable();
                if (this.bladeForm.controls[data.switchFormControlName].value) {
                    this.bladeForm.controls[data.formControlName].enable();
                }
            } else if (isManual && isManual !== 'Manual') {
                this.bladeForm.controls[data.formControlName].disable();
                this.bladeForm.controls[data.switchFormControlName].disable();
                this.outer.emit(null);
            }
        });
        if (changes.capValue && changes.capValue.currentValue) {
            this.refreshVerification();
        }
    }
}

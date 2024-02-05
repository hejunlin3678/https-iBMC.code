import { UserInfoService, AlertMessageService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { Subscription } from 'rxjs';
import { Component, OnInit, Input } from '@angular/core';
import { UntypedFormBuilder, UntypedFormControl, ValidationErrors } from '@angular/forms';
import { TiTableColumns, TiTableSrcData, TiValidators, TiMessageService } from '@cloud/tiny3';
import { LoginRuleService } from './service/login-rule.service';
import LoginRuleValidator from './validator';
import { SecurityService } from '../../service';
import { ITableRow } from './login-rule.datatype';

@Component({
    selector: 'app-login-rule',
    templateUrl: './login-rule.component.html',
    styleUrls: ['./login-rule.component.scss']
})
export class LoginRuleComponent implements OnInit {

    @Input() systemLockDownEnabled: boolean;

    public isOemSecurity = this.userServ.hasPrivileges(['OemSecurityMgmt']);

    public displayed: ITableRow[] = [];

    public srcData: TiTableSrcData;

    private i18n = {
        VALIDATOR_TIME_FORMAT_INCORRECT: this.translate.instant('VALIDATOR_TIME_FORMAT_INCORRECT'),
        VALIDATOR_START_ENDING: this.translate.instant('VALIDATOR_START_ENDING'),
        VALIDATOR_RIGHT_TIME1: this.translate.instant('VALIDATOR_RIGHT_TIME1'),
        VALIDATOR_RIGHT_TIME2: this.translate.instant('VALIDATOR_RIGHT_TIME2'),
        VALIDATOR_WRONG_TIME1: this.translate.instant('VALIDATOR_WRONG_TIME1'),
        VALIDATOR_WRONG_TIME2: this.translate.instant('VALIDATOR_WRONG_TIME2'),
        VALIDATOR_MAC_ERROR: this.translate.instant('VALIDATOR_MAC_ERROR'),
        VALIDATOR_INCORRECT: this.translate.instant('VALIDATOR_INCORRECT'),
    };

    private validator = new LoginRuleValidator(this.i18n);

    private getRuleSub: Subscription;

    private saveParamSub: Subscription;

    private formerTableData: object = {};

    constructor(
        private loginRuleServ: LoginRuleService,
        private formBuilder: UntypedFormBuilder,
        private translate: TranslateService,
        private securityServ: SecurityService,
        private tiMessage: TiMessageService,
        private alertServ: AlertMessageService,
        private userServ: UserInfoService,
    ) { }

    public columns: TiTableColumns[] = [
        {
            title: 'COMMON_NAME',
            width: '8%'
        },
        {
            title: 'SECURITY_PERIOD',
            width: '40%',
            iconTip: 'SECURITY_TIME_SEGMENT_SUPPORTED',
        },
        {
            title: 'SECURITY_IP_SEGMENT',
            width: '15%',
            iconTip: 'SECURITY_IP_SEGMENT_SUPPORTED',
        },
        {
            title: 'SECURITY_MAC_SEGMENT',
            width: '17%',
            iconTip: 'SECURITY_MAC_SEGMENT_SUPPORTED',
        },
        {
            title: 'STATUS',
            width: '15%'
        },
        {
            title: 'COMMON_OPERATE',
            width: '170px'
        }
    ];

    public trackByFn(index: number, item: any): string {
        return item.memberId;
    }

    public ruleSave(row: ITableRow): void {
        const formErrors: ValidationErrors | null = TiValidators.check(row.timeForm);
        const otherErros = row.ipValidation.errors || row.macValidation.errors;
        if (formErrors || otherErros) {
            return;
        }

        const content = this.securityServ.userSelectId ?
            this.translate.instant('VALIDATOR_URGENT_USER') : this.translate.instant('VALIDATOR_NO_URGENT_USER');

        const msgModal = this.tiMessage.open({
            id: 'ruleSaveModal',
            type: 'prompt',
            closeIcon: false,
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                autofocus: false,
                primary: true,
                click: () => {
                    msgModal.close();
                    this.saveParamSub = this.loginRuleServ.saveRule(row).subscribe((data) => {
                        row.startTime = row.timeForm.controls.startTime.value;
                        row.endTime = row.timeForm.controls.endTime.value;
                        row.mac = row.macValidation.value;
                        row.IP = row.ipValidation.value;
                        row.isEdit = false;

                        this.alertServ.eventEmit.emit({
                            type: 'success',
                            label: 'COMMON_SUCCESS',
                        });

                        this.saveParamSub.unsubscribe();
                    });
                },
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('COMMON_CANCEL')
            },
            title: this.translate.instant('COMMON_CONFIRM'),
            content
        });

    }

    public changeTime(row: ITableRow) {
        // 时间更改时，要更新相互校验的传值，另外因为校验比ngModelChange要快，所以需要重新更新自身校验，以便再走一遍自身校验逻辑
        row.timeForm.setControl('endTime', new UntypedFormControl(row.timeForm.controls.endTime.value, [
            this.validator.selfTimeValid, this.validator.interTimeValid(false, row.timeForm.controls.startTime.value)
        ]));
        row.timeForm.setControl('startTime', new UntypedFormControl(row.timeForm.controls.startTime.value, [
            this.validator.selfTimeValid, this.validator.interTimeValid(true, row.timeForm.controls.endTime.value)
        ]));

        row.timeForm.get('endTime').updateValueAndValidity();
        row.timeForm.get('startTime').updateValueAndValidity();
    }

    public ruleCancel(row: ITableRow): void {
        row.isEdit = false;
        const formerRowData = this.formerTableData[row.memberId];
        row.timeForm.controls.startTime.setValue(formerRowData.startTime);
        row.timeForm.controls.endTime.setValue(formerRowData.endTime);
        row.ipValidation.setValue(row.IP);
        row.macValidation.setValue(row.mac);
        row.ruleEnabled = formerRowData.ruleEnabled;
    }

    public ruleEdit(row: ITableRow): void {
        if (this.isOemSecurity) {
            this.formerTableData[row.memberId] = Object.assign({}, row);
            row.isEdit = true;
        }
    }

    private addValidation(item: ITableRow): ITableRow {
        const timeForm = this.formBuilder.group({
            startTime: new UntypedFormControl(
                item.startTime, [this.validator.selfTimeValid, this.validator.interTimeValid(true, item.endTime)]),
            endTime: new UntypedFormControl(item.endTime,
                [this.validator.selfTimeValid, this.validator.interTimeValid(false, item.startTime)]),
        });
        const ipValidation = new UntypedFormControl(item.IP, this.validator.ipValid);
        const macValidation = new UntypedFormControl(item.mac, this.validator.macValid);
        return Object.assign(item, {timeForm, ipValidation, macValidation});
    }

    ngOnInit() {
        this.getRuleSub = this.loginRuleServ.getData().subscribe((data) => {
            const dataWithValidation: ITableRow[] = data.map((item: ITableRow) => this.addValidation(item));
            this.srcData = {
                data: dataWithValidation,
                state: {
                    searched: false,
                    sorted: false,
                    paginated: false
                }
            };
            this.getRuleSub.unsubscribe();
        });
    }

    ngOnDestroy(): void {
        this.getRuleSub.unsubscribe();
        if (this.saveParamSub && this.saveParamSub.unsubscribe) {
            this.saveParamSub.unsubscribe();
        }
    }
}

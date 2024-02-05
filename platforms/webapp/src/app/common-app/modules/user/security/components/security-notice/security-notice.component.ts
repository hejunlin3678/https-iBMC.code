import { Component, OnInit, Input } from '@angular/core';
import { Subscription } from 'rxjs';
import { UserInfoService, AlertMessageService } from 'src/app/common-app/service';
import { SecurityNoticeService } from './service';
import { ISecurityNoticeData } from './security-notice.datatype';
import { cutStr, getByte } from 'src/app/common-app/utils';

@Component({
    selector: 'app-security-notice',
    templateUrl: './security-notice.component.html',
    styleUrls: ['./security-notice.component.scss']
})
export class SecurityNoticeComponent implements OnInit {

    @Input() systemLockDownEnabled: boolean;

    constructor(
        private userServ: UserInfoService,
        private noticeServ: SecurityNoticeService,
        private alertServ: AlertMessageService,
    ) { }

    public isOemSecurity = this.userServ.hasPrivileges(['OemSecurityMgmt']);

    public buttonDisable = true;

    private userInfoSub: Subscription;

    private saveParamSub: Subscription;

    public readonly MAX_TEXTAREA_LENGTH = 1024;

    public extraSize: string;

    public switch = {
        id: 'noticeSwitch',
        label: 'SECURITY_LOGIN_ENABLED',
        disabled: !this.isOemSecurity,
        value: false,
        formerValue: false,
        change: () => {
            this.detectChange();
        },
    };

    public textarea = {
        id: 'noticeTextarea',
        label: 'SECURITY_SECURITY_MESSAGE',
        value: '',
        formerValue: '',
        defaultValue: '',
        disabled: !this.isOemSecurity,
        change: (value: string) => {
            const extraSize = getByte(value);
            // 修改自己的内容需要是异步操作
            if (extraSize > this.MAX_TEXTAREA_LENGTH) {
                setTimeout(() => {
                    this.textarea.value = cutStr(this.textarea.value, this.MAX_TEXTAREA_LENGTH);
                    this.detectChange();
                    this.extraSize = this.getExtraSize();
                }, 0);
            }
            this.detectChange();
            this.extraSize = String(extraSize);
        },
    };

    private formComps = [this.switch, this.textarea];

    private getExtraSize(extraSize?: number): string {
        if (extraSize === undefined) {
            extraSize = getByte(this.textarea.value);
        }
        return String(extraSize);
    }

    ngOnInit() {
        this.init();
    }

    private init() {
        this.userInfoSub = this.noticeServ.getData().subscribe((data: ISecurityNoticeData) => {
            this.initValue(data);
        });
    }

    private initValue(data: ISecurityNoticeData) {
        this.switch.value = this.switch.formerValue = data.status;
        this.textarea.value = this.textarea.formerValue = data.value;
        this.textarea.defaultValue = data.defaultValue;
        this.extraSize = this.getExtraSize();
    }

    private detectChange() {
        this.buttonDisable = this.systemLockDownEnabled || !this.formComps.some((comp) => comp.value !== comp.formerValue);
    }

    public save() {
        const changedObj: ISecurityNoticeData = {};
        if (this.switch.value !== this.switch.formerValue) {
            changedObj.status = this.switch.value;
        }
        if (this.textarea.value !== this.textarea.formerValue) {
            changedObj.value = this.textarea.value;
        }
        this.saveParamSub = this.noticeServ.saveParams(changedObj).subscribe((data) => {
            this.alertServ.eventEmit.emit({
                type: 'success',
                label: 'COMMON_SUCCESS'
            });
            this.initValue(data);
            this.saveParamSub.unsubscribe();
            this.buttonDisable = true;
        }, () => {
            this.saveParamSub.unsubscribe();
        });
    }

    public reset() {
        this.textarea.value = this.textarea.defaultValue;
        this.textarea.change(this.textarea.defaultValue);
    }

    ngOnDestroy(): void {
        this.userInfoSub.unsubscribe();
    }

}

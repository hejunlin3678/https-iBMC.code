import { Component, Input } from '@angular/core';
import { LocalUserService } from '../../localuser.service';
import { cutStr, getMessageId, getByte } from 'src/app/common-app/utils';
import { TranslateService } from '@ngx-translate/core';
import { TiFileItem, TiFileInfo } from '@cloud/tiny3';
import { SecondAuthorityFail, FileAddFaild } from '../../../dataType/data.model';
import { IUsersInfo } from '../../../dataType/data.interface';
import { AlertMessageService, UserInfoService } from 'src/app/common-app/service';


@Component({
    selector: 'app-ssh-upload',
    templateUrl: './ssh-upload.component.html',
    styleUrls: ['./ssh-upload.component.scss']
})
export class SshUploadComponent {
    @Input() activeRow: IUsersInfo = null;
    constructor(
        private alertService: AlertMessageService,
        private localService: LocalUserService,
        private userInfo: UserInfoService,
        private translate: TranslateService
    ) { }
    public MAXLENGTH: number = 2048;
    public radioList: { key: string; id: string; }[] = [
        {
            key: 'USER_SSH_PUBLIC_FILE',
            id: 'publicFile'
        }, {
            key: 'USER_SSH_PUBLIC_TEXT',
            id: 'publicText'
        }
    ];
    public selected = this.radioList[0].id;
    public loginWithoutPassword: boolean = this.userInfo.loginWithoutPassword;
    // 保存按钮状态
    public disabled: boolean = true;
    public loginPwd: string = '';
    public publicText: string = '';
    public currentLenth: number = 0;
    public isAddFileError: boolean = false;

    // 文件上传
    public errorTips: string = '';
    public uploaderInstance: TiFileItem = null;
    public sshFormData: FormData = null;
    public isPwdError: boolean = false;

    public uploaderConfig = {
        url: '',
        type: 'POST',
        filters: [{
            name: 'maxCount',
            params: [1]
        }, {
            name: 'type',
            params: ['.pub']
        }, {
            name: 'maxSize',
            params: [2 * 1024]
        }]
    };

    public onAddItemFailed(error: { file: TiFileInfo, validResults: string[] }): void {
        this.isAddFileError = true;
        this.uploaderInstance = null;
        if (error.validResults[0] === FileAddFaild.TYPE) {
            this.errorTips = 'USER_SSH_ADD_FAILED_TYPE';
        }
        if (error.validResults[0] === FileAddFaild.MAXSIZE) {
            this.errorTips = 'USER_SSH_ADD_FAILED_MAXSIZE';
        }
    }

    public onAddItemSuccess(fileItem: TiFileItem): void {
        this.isAddFileError = false;
        this.uploaderInstance = fileItem;
        this.sshFormData = null;

        const sshData: FormData = new FormData();
        sshData.append('Content', fileItem._file, fileItem.file.name);
        sshData.append('Type', 'URI');

        this.sshFormData = sshData;
        this.disabled = false;
    }

    public onRemoveItems(): void {
        this.isAddFileError = false;
        this.disabled = true;
    }

    public pwdChange(): void {
        this.isPwdError = false;
    }

    // 上传公钥文件或公钥信息文本
    public upload(): void {
        if (this.disabled) {
            return;
        }

        if (!this.loginWithoutPassword && this.loginPwd.length === 0) {
            return;
        }

        let formData: FormData = null;

        if (this.selected !== this.radioList[0].id) {
            formData = new FormData();
            formData.append('Content', this.publicText);
            formData.append('Type', 'Text');
        } else {
            formData = this.sshFormData;
        }
        if (this.loginPwd !== '') {
            formData.append('ReauthKey', window.btoa(this.loginPwd));
        }

        this.disabled = true;
        this.localService.uploadFile({
            userId: this.activeRow.id,
            formData,
            secPwd: this.loginPwd
        }).subscribe({
            next: () => {
                this.alertService.eventEmit.emit({
                    type: 'success',
                    label: this.translate.instant('COMMON_SUCCESS')
                });
                this.close();
            },
            error: (error) => {
                this.disabled = false;
                this.loginPwd = '';
                const errorId = getMessageId(error.error)[0].errorId;
                if (errorId === SecondAuthorityFail.AUTHORIZATIONFAILED) {
                    this.isPwdError = true;
                    return;
                }
                this.alertService.eventEmit.emit({
                    type: 'error',
                    label: this.translate.instant(errorId)['errorMessage'] || 'COMMON_FAILED'
                });
                this.dismiss();
            }
        });
    }

    public cancel(): void {
        this.dismiss();
    }

    // 空方法不要删除，没有这2个方法，tiny3弹框组件不会关闭
    public close(): void {

    }

    public dismiss(): void {

    }

    // 字节计算
    public computeBytes(): void {
        this.publicText = cutStr(this.publicText, this.MAXLENGTH);
        this.currentLenth = getByte(this.publicText);
        this.disabled = this.currentLenth === 0;
    }

    // radio切换
    public switchRadio(): void {
        this.publicText = '';
        this.loginPwd = '';
        this.isPwdError = false;
        this.currentLenth = 0;
        this.isAddFileError = false;
        if (this.uploaderInstance) {
            this.uploaderInstance.remove();
            this.uploaderInstance = null;
        }
        this.disabled = true;
    }
}

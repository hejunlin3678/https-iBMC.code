import { Component, OnInit } from '@angular/core';
import { UserInfoService, LoadingService, AlertMessageService } from 'src/app/common-app/service';
import { UntypedFormBuilder } from '@angular/forms';
import { SpService } from './services/sp.service';
import { TiMessageService } from '@cloud/tiny3';
import { TranslateService } from '@ngx-translate/core';
@Component({
    selector: 'app-sp',
    templateUrl: './sp.component.html',
    styleUrls: ['./sp.component.scss']
})
export class SpComponent implements OnInit {

    constructor(
        private user: UserInfoService,
        private fb: UntypedFormBuilder,
        private spService: SpService,
        private tiMessage: TiMessageService,
        private translate: TranslateService,
        private alert: AlertMessageService,
        private loading: LoadingService
    ) { }

    public isPrivileges = this.user.hasPrivileges(['ConfigureComponents']);
    public isSystemLock = this.user.systemLockDownEnabled;

    public spParams: object = {};
    public spData: object = {};

    public showAlert = {
        type: 'prompt',
        openAlert: true,
        closeIcon: false
    };

    public spOptions = {
        device: Symbol(),
        OS: Symbol()
    };

    public spForm = this.fb.group({
        enableSwitchState: [{ value: null, disabled: false }, []],
        enableStartState: [{ value: null, disabled: false }, []]
    });


    private init(spData) {
        this.spParams['enabbleState'] = spData.SPDeviceInfoCollectEnabled;
        this.spParams['startEnabled'] = spData.SPStartEnabled;
        this.spForm.patchValue({
            enableSwitchState: this.spParams['enabbleState'],
            enableStartState: this.spParams['startEnabled']
        });
    }
    ngOnInit(): void {
        // 权限控制具有安全配置权限可以设置vmm使能、端口
        if (this.isPrivileges && !this.isSystemLock) {
            this.spForm.root.get('enableSwitchState').enable();
            this.spForm.root.get('enableStartState').enable();
        } else {
            this.spForm.root.get('enableSwitchState').disable();
            this.spForm.root.get('enableStartState').disable();
        }

        this.spService.getSp().subscribe((response) => {
            this.spData = response['body'];
            this.init(this.spData);
        }, () => {
            this.loading.state.next(false);
        }, () => {
            this.loading.state.next(false);
        });
    }

    public spEnabledChange = (option) => {
        const instance = this.tiMessage.open({
            id: 'picButton',
            type: 'prompt',
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    const queryParams = {};
                    if (option === this.spOptions.device) {
                        queryParams['SPDeviceInfoCollectEnabled'] = !this.spForm.root.get('enableSwitchState').value;
                    }
                    if (option === this.spOptions.OS) {
                        queryParams['SPStartEnabled'] = !this.spForm.root.get('enableStartState').value;
                    }
                    this.spService.setSp(JSON.stringify(queryParams)).subscribe((response) => {
                        this.alert.eventEmit.emit({type: 'success', label: 'COMMON_SUCCESS'});
                        this.spData = response['body'];
                        this.init(this.spData);
                    }, (error) => {
                        this.alert.eventEmit.emit({ type: 'error', label: 'COMMON_FAILED' });
                        this.spData = error.error.data;
                        this.init(this.spData);
                    });
                    instance.close();
                }
            },
            cancelButton: {
                show: true,
                text: this.translate.instant('ALARM_CANCEL'),
                click: () => {
                    instance.dismiss();
                }
            },
            title: this.translate.instant('ALARM_OK'),
        });
    }

}

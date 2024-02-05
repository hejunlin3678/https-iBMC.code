import { Component, OnInit } from '@angular/core';
import { openKvm } from './models/kvmclient';
import { TranslateService } from '@ngx-translate/core';
import { TiMessageService } from '@cloud/tiny3';
import { CommonService, HttpService, LanguageService } from 'src/app/common-app/service';
import { DomSanitizer } from '@angular/platform-browser';
import { HttpResponse } from '@angular/common/http';
import { Router } from '@angular/router';

@Component({
    selector: 'app-kvm-h5',
    templateUrl: './kvm-h5.component.html',
    styleUrls: ['./kvm-h5.component.scss']
})
export class KvmH5Component implements OnInit {
    private routerParams;
    constructor(
        private common: CommonService,
        private http: HttpService,
        private translate: TranslateService,
        private tiMessage: TiMessageService,
        private domSanitizer: DomSanitizer,
        private lang: LanguageService,
        private router: Router,
    ) {
        this.routerParams = this.router.getCurrentNavigation().extras.state;
    }
    public scope = {
        currnetLang: this.lang.locale.toLowerCase(),
        fullscreenTitle: 'REMOTE_FULL_TIP',
        translate: this.translate,
        domSanitizer: this.domSanitizer,
        slider: {
            scales: [],
            limits: {
                min: 0,
                max: 100
            },
            value: 70,
            step: [],
            tooltip: {
                formatter(value) {
                    return value + '%';
                }
            },
            change(value: number) { },
            changeStop(value: number) { }
        },
        cdDvd: [{
            key: 'HOME_IMAGE_FILE',
            id: 'iso',
            disable: false
        }, {
            key: 'HOME_LOCAL_DIRECTORY',
            id: 'location',
            disable: false
        }],
        cdDvdSelected: {
            value: 'iso'
        },
        isoCdrom: {
            uploaderConfig: {
                url: 'myUploadUrl',
                isAutoUpload: false,
                filters: [{
                    name: 'type',
                    params: ['.iso']
                },
                {
                    name: 'maxCount',
                    params: [1]
                }],
                onAddItemSuccess(fileItem) { },
                onRemoveItems: (fileItem) => {
                    this.scope._cdromConnect.disable = true;
                }
            },
            styleOptions: {
                showSubmitBtn: false,
                disable: false
            },
            upload() { }
        },
        _cdromConnect: {
            text: 'HOME_CONNECT',
            disable: true,
            click() { }
        },
        locationFile: {
            uploaderConfig: {
                url: 'myUploadUrl',
                isAutoUpload: false,
                filters: [{
                    name: 'maxCount',
                    params: [1]
                }],
                onAddItemSuccess(fileItem) { },
                onRemoveItems: (fileItem) => {
                    this.scope._cdromConnect.disable = true;
                    this.scope._cdromInsert.disable = true;
                }
            },
            styleOptions: {
                showSubmitBtn: false,
                disable: true
            },
            upload() { }
        },
        _cdromInsert: {
            text: 'HOME_INSERT',
            disable: true,
            click() { }
        },
        floppy: [{
            key: 'HOME_IMAGE_FILE',
            id: 'iso_floppy',
            disable: false
        }],
        isoFloppy: {
            uploaderConfig: {
                url: 'myUploadUrl',
                isAutoUpload: false,
                filters: [{
                    name: 'type',
                    params: ['.img']
                },
                {
                    name: 'maxCount',
                    params: [1]
                }
                ],
                onAddItemSuccess(fileItem) { },
                onRemoveItems: (fileItem) => {
                    this.scope._floppyConnect.disable = true;
                }
            },
            styleOptions: {
                showSubmitBtn: false,
                disable: false
            },
            upload() { }
        },
        _floppyConnect: {
            text: 'HOME_CONNECT',
            disable: true,
            click() { }
        },
        _floppyInsert: {
            text: 'HOME_INSERT',
            disable: true,
            click() { }
        },
        rChange() { },
        kvmParams: null,
        tiMessage: this.tiMessage
    };

    init() {
        if (localStorage.getItem('H5ClientModel')) {
            const queryParams = {
                Mode: localStorage.getItem('H5ClientModel'),
                Language: this.lang.locale.split('-')[0]
            };
            const headers = {
                'Content-Type': 'application/json;charset=utf-8',
                'Accept': 'application/json'
            };
            this.http.post(
                '/UI/Rest/Services/KVM/GenerateStartupFile',
                queryParams,
                {
                    headers,
                    responseType: 'text'
                }
            ).subscribe((response) => {
                this.scope.kvmParams = this.common.loadXML(response.body);
                openKvm(this.scope, this.showMessage);
            }, (error) => {
                this.showMessage('REMOTE_ERR_LINK_FAILED');
            });
            localStorage.removeItem('H5ClientModel');
        } else if (this.routerParams?.kvmHtml5Info) {
            this.scope.kvmParams = JSON.parse(this.routerParams?.kvmHtml5Info);
            if (this.routerParams?.kvmHtml5Info) {
                this.scope.kvmParams['bladesize'] = '' + this.scope.kvmParams['bladesize'];
                this.scope.kvmParams['mmVerifyValue'] = '' + this.scope.kvmParams['mmVerifyValue'];
                this.scope.kvmParams['port'] = '' + this.scope.kvmParams['port'];
                this.scope.kvmParams['privilege'] = '' + this.scope.kvmParams['privilege'];
                this.scope.kvmParams['verifyValue'] = '' + this.scope.kvmParams['verifyValue'];
                this.scope.kvmParams['vmmPort'] = '' + this.scope.kvmParams['vmmPort'];
            }
            openKvm(this.scope, this.showMessage);
            setTimeout(() => {
                localStorage.removeItem('kvmHtml5Info');
                sessionStorage.removeItem('kvmHtml5Info');
            }, 3000);
        } else if (sessionStorage.getItem('kvmHtml5Info') || localStorage.getItem('kvmHtml5Info')) {
            /**
             * 非web打开KVM(SSO,RedFish,IPMI);
             * 参照web打开时,kvmParams所需参数如下：
             * SN,verifyValue,mmVerifyValue,decrykey,local,compress,vmm_compress,port,vmmPort,privilege,bladesize,IPA,IPB,verifyValueExt
             * 2022-4-25修改，变更如下
             * 1. decrykey从SSOHandler接口去获取，其他数据依然从LoccalStorage获SessionStorage获取
             */
            const locHtml5Json = JSON.parse(sessionStorage.getItem('kvmHtml5Info')) || JSON.parse(localStorage.getItem('kvmHtml5Info'));
            const redirect = locHtml5Json.redirect;
            const lang = locHtml5Json.lang;
            const kvmmode = locHtml5Json.kvmmode;
            const openway = locHtml5Json.openway;
            let url = '';
            if (redirect) {
                url = `/UI/Rest/SSOHandler?redirect=${redirect}&lang=${lang}&kvmmode=${kvmmode}&openway=${openway}&token=header`;
            } else {
                url = `/UI/Rest/KVMHandler?&authParam=header`;
                if (locHtml5Json.openWay) {
                    url += `&openWay=${locHtml5Json.openWay}`;
                }
                if (locHtml5Json.lp) {
                    url += `&lp=${locHtml5Json.lp}`;
                }
                if (locHtml5Json.lang) {
                    url += `&lang=${locHtml5Json.lang}`;
                }
            }
            this.http.get(url).subscribe((res: HttpResponse<any>) => {
                this.scope.kvmParams = res.body.kvmHtml5Info;
                openKvm(this.scope, this.showMessage);
                setTimeout(() => {
                    localStorage.removeItem('kvmHtml5Info');
                    sessionStorage.removeItem('kvmHtml5Info');
                }, 3000);
            }, () => {
                localStorage.setItem('ssoError', 'error');
                this.router.navigate(['navigate']);
            });
        } else {
            this.showMessage('REMOTE_ERR_SESSION_EXPIRATION');
        }
    }
    public showMessage = (contentInfo) => {
        const instance = this.tiMessage.open({
            type: 'warn',
            id: 'kvmWarnModal',
            content: this.translate.instant(contentInfo),
            title: this.translate.instant('HOME_PERFWARNING'),
            okButton: {
                show: true,
                text: this.translate.instant('COMMON_OK'),
                disabled: false,
                autofocus: false,
                click() {
                    instance.close();
                }
            },
            cancelButton: {
                show: false
            }
        });
    }

    ngOnInit(): void {
        this.init();
    }
}

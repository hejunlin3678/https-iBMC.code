import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import {
    LanguageService,
    AlertMessageService,
    LoadingService,
    SystemLockService,
    CommonService,
    ErrortipService,
} from 'src/app/common-app/service';
import { LanguageManageService } from './services';
import { TiMessageService, TiTableSrcData } from '@cloud/tiny3';
import { getMessageId } from 'src/app/common-app/utils';
@Component({
    selector: 'app-language',
    templateUrl: './language.component.html',
    styleUrls: ['../manager.component.scss', './language.component.scss']
})
export class LanguageComponent implements OnInit {

    constructor(
        private translate: TranslateService,
        private langService: LanguageService,
        private languageManageService: LanguageManageService,
        private tiMessage: TiMessageService,
        private alert: AlertMessageService,
        private loading: LoadingService,
        private commonService: CommonService,
        private lockService: SystemLockService,
        private errorTipService: ErrortipService
    ) { }

    public isSystemLock: boolean = false;
    public deleteUrl: string;
    public columns = [{
        title: 'LANGUAGE_NUMBER',
        width: '25%'
    },
    {
        title: 'LANGUAGE_CODE',
        width: '25%'
    },
    {
        title: 'LANGUAGE_NAME',
        width: '25%'
    },
    {
        title: 'LANGUAGE_OPERATION',
        width: '25%'
    }];
    public displayed = [];
    public srcData: TiTableSrcData = {
        data: [],
        state: {
            searched: false,
            sorted: false,
            paginated: false
        }
    };
    public languages = [];
    public lockStateChange(state: boolean) {
        if (this.isSystemLock !== state) {
            this.isSystemLock = state;
        }
    }
    public init() {
        this.loading.state.next(true);
        this.languageManageService.getLangInfo().subscribe((res: any) => {
            const response = res['body'];
            this.languages = response.LanguageSet.split(',');
            const arr = [];
            this.langService.langOptions.forEach((item, index) => {
                arr.push({
                    languageNumber: index + 1,
                    languageCode: item.langSet,
                    languageName: item.label,
                    languageEnable: this.languages.indexOf(item.langSet) > -1,
                    operation: (item.langSet === 'zh' || item.langSet === 'en') ? false : true
                });
            });
            this.srcData.data = arr;
            this.loading.state.next(false);
        });
    }
    public switchLangFun(lang: string) {
        this.alert.eventEmit.emit({ type: 'success', label: 'COMMON_SUCCESS' });
            this.commonService.getGenericInfo().subscribe((res) => {
                this.langService.autoExtracsLang(res.body.LanguageSet);
            });
            const locale = this.langService.getTargetLanguage();
            if (locale.indexOf(lang) > -1) {
                setTimeout(() => {
                    window.location.reload();
                }, 1000);
            } else {
                this.init();
            }
    }
    public setLangErrorFun(error: any) {
        const errorId = getMessageId(error.error)[0].errorId;
        const errorMessage = this.errorTipService.getErrorMessage(errorId) || 'COMMON_FAILED';
        this.alert.eventEmit.emit({ type: 'error', label: errorMessage });
    }
    public switchLangConfirm(lang: string) {
        const params = { 'LanguageSetStr': lang };
        if (this.languages.indexOf(lang) > -1) {
            this.languageManageService.closeLang(params).subscribe(response => {
                this.switchLangFun(lang);
            }, (error) => {
                this.setLangErrorFun(error);
            });
        } else {
            this.languageManageService.openLang(params).subscribe(response => {
                this.switchLangFun(lang);
            }, (error) => {
                this.setLangErrorFun(error);
            });
        }
    }
    public langEnabledChange(lang: string) {
        const instance = this.tiMessage.open({
            id: 'picButton',
            type: 'prompt',
            content: this.translate.instant('COMMON_ASK_OK'),
            okButton: {
                show: true,
                text: this.translate.instant('ALARM_OK'),
                autofocus: false,
                click: () => {
                    this.switchLangConfirm(lang);
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

    ngOnInit(): void {
        this.loading.state.next(true);
        // 系统锁定功能
        this.isSystemLock = this.lockService.getState();
        this.lockService.lockStateChange.subscribe(res => {
            this.isSystemLock = res;
        });
        this.init();
    }

}

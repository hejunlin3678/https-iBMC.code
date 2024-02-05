import { Injectable } from '@angular/core';
import { ActivatedRouteSnapshot, RouterStateSnapshot, UrlTree, CanActivate } from '@angular/router';
import { Observable, forkJoin } from 'rxjs';
import { UserInfoService } from 'src/app/common-app/service/user-info.service';
import { GlobalDataService } from 'src/app/common-app/service/global-data.service';
import { LanguageService } from 'src/app/common-app/service';
import { TranslateService } from '@ngx-translate/core';
import { TiLocale } from '@cloud/tiny3';
import { GlobalData, CommonData } from 'src/app/common-app/models';
// 加载错误处理包和tiny3语言包
import * as zh from 'src/assets/common-assets/i18n/zh-CN';
import * as en from 'src/assets/common-assets/i18n/en-US';
import * as ja from 'src/assets/common-assets/i18n/ja-JA';
import * as fr from 'src/assets/common-assets/i18n/fr-FR';
import * as ru from 'src/assets/common-assets/i18n/ru-RU';
import { map } from 'rxjs/internal/operators/map';
import { AppService } from 'src/app/app.service';
@Injectable({
    providedIn: 'root'
})
export class AppGuard implements CanActivate {
    constructor(
        private userInfo: UserInfoService,
        private appServer: AppService,
        private langService: LanguageService,
        private translate: TranslateService,
        private globalData: GlobalDataService
    ) {
    }
    canActivate(
        next: ActivatedRouteSnapshot,
        state: RouterStateSnapshot): Observable<boolean | UrlTree> | Promise<boolean | UrlTree> | boolean | UrlTree {
        return new Observable(observe => {
            // 初始化userInfo, localstorage如果存在的话
            const userData = JSON.parse(localStorage.getItem('userInfo'));
            if (userData) {
                Object.keys(userData).forEach((key) => {
                    this.userInfo[key] = userData[key];
                });
            }
            // tiny3语言扩展
            this.extendLanguage();
            // 通过发送请求来设置产品类型
            this.globalData.webType = new Promise((resolve, reject) => {
                this.initSystemBaseInfo(resolve, observe);
            });
        });
    }
    // 异步查询V1请求数据，根据V1的结果在执行后续操作
    private async initSystemBaseInfo(resolve, observe) {
        await this.getV1Info();
        // 设置语言字段
        const locale = this.langService.getTargetLanguage();
        this.translate.addLangs([locale]);
        this.langService.locale = locale;
        this.langService.setLangObject(locale);

        // 加载错误处理语言包
        await this.loadErrorAndSuccessMessage();

        // 设置最终的语言文件内容
        this.translate.use(locale);
        TiLocale.setLocale(locale);

        // 加载CSS文件
        this.setExtrasCss(locale);

        // 初始化访问时清空cookie中的tiny_language信息(使其过期)
        document.cookie = 'tiny_language=0;expires=' + new Date(1970, 0, 1);
        observe.next(true);
        observe.complete();
        resolve();
    }

    // 异步获取V1请求函数
    public async getV1Info() {
        return new Promise((resolve, reject) => {
            this.appServer.getInitInfo().subscribe((res) => {
                const v1Info = res.v1Info || {};
                this.globalData.productName = v1Info.ProductName;
                this.globalData.fqdn = v1Info.FQDN;
                this.globalData.copyright = v1Info.Copyright;
                if (this.globalData.copyright) {
                    this.globalData.copyright = 'Copyright © ' + this.globalData.copyright;
                }
                this.globalData.downloadKVMLink = v1Info.DownloadKVMLink;
                GlobalData.getInstance().setSoftwareName = String(v1Info.SoftwareName);
                GlobalData.getInstance().setSmsName = String(v1Info.SmsName || 'iBMA');
                localStorage.setItem('softwareName', v1Info.SoftwareName);
                // 浏览器tab 标签名称
                document.title = localStorage.getItem('softwareName') + ' ' + window.location.hostname;
                // 把不支持的语言列表隐藏
                this.langService.showExtracsLang(v1Info.LanguageSet);
                resolve(null);
            });
        });
    }
    // 将错误的和正确的国际化语言包追加到 translate里面
    private async loadErrorAndSuccessMessage() {
        const translateArr = [];
        this.translate.getLangs().forEach((lang) => {
            translateArr.push(this.getTranslation(lang));
        });
        return new Promise((resolve) => {
            forkJoin(translateArr).subscribe(() => {
                resolve(null);
            });
        });
    }
    // 通过修改body得class来实现针对特定语言的样式hack
    private setExtrasCss(lang) {
        document.querySelector('#app').className = lang;
    }
    private getTranslation(lang: string): Observable<void> {
        const relationError = {
            'zh-CN': zh.errorMessage,
            'en-US': en.errorMessage,
            'ja-JA': ja.errorMessage,
            'fr-FR': fr.errorMessage,
            'ru-RU': ru.errorMessage
        };
        const relationSuccess = {
            'zh-CN': zh.successMessage,
            'en-US': en.successMessage,
            'ja-JA': ja.successMessage,
            'fr-FR': fr.successMessage,
            'ru-RU': ru.successMessage
        };

        return this.translate.getTranslation(lang).pipe(
            map((res) => {
                const mergedTrans = Object.assign({}, res, relationError[lang], relationSuccess[lang]);
                const softwareName: string = GlobalData.getInstance().getSoftwareName;
                const smsName: string = GlobalData.getInstance().getSmsName;
                const i18nStr = JSON.stringify(mergedTrans).replace(/{softwareName}/g, softwareName).replace(/{smsName}/g, smsName);
                this.translate.setTranslation(lang, JSON.parse(i18nStr));
            })
        );
    }
    // 设置tiny3的语言库
    private extendLanguage() {
        TiLocale.setWords({
            'zh-CN': zh.tiny3Entries,
            'en-US': en.tiny3Entries,
            'ja-JA': ja.tiny3Entries,
            'fr-FR': fr.tiny3Entries,
            'ru-RU': ru.tiny3Entries
        });
    }
}

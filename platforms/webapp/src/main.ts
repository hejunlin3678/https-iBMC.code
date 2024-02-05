import { enableProdMode } from '@angular/core';
import { platformBrowserDynamic } from '@angular/platform-browser-dynamic';

import { AppModule } from './app/app.module';
import { environment } from './environments/environment';
import cssVars from 'css-vars-ponyfill';
import { getBrowserType } from './app/common-app/utils';

/**
 * IE访问IPV6，需要转换IP格式，否则无法使用KVM和video
 * 转换规则：冒号(:)转为横杠(-),%转为s，去掉[]，末尾加.ipv6-literal.net
 */
let literalIp = location.hostname;
if ((getBrowserType().browser === 'ie') && literalIp.indexOf(':') > -1) {
    literalIp = literalIp.replace(/:/g, '-');
    literalIp = literalIp.replace(/\[/g, '');
    literalIp = literalIp.replace(/\]/g, '');
    literalIp = literalIp.replace(/%/g, 's');
    literalIp += '.ipv6-literal.net';
    window.location.href = 'https://' + literalIp;
}
if (environment.production) {
    enableProdMode();
}

cssVars({ watch: true });
platformBrowserDynamic().bootstrapModule(AppModule).catch(error => console.log(error));

const kvmMark = ['remoteconsole', 'kvmvmm.asp'];
const search = window.location.search;
const pathname = window.location.pathname;
if (search.indexOf('java') > -1 || search.indexOf('jre') > -1) {
    localStorage.setItem('kvmJava', '1');
}
const getUrlParams = (param, urlVars) => {
    for (const iterator of urlVars) {
        const item = iterator.split('=');
        if (item[0] === param) {
            return item[1].toLowerCase();
        }
    }
};
if (search) {
    const urlVars: string[] = search.split('?')[1].split('&');
    const language = getUrlParams('lp', urlVars);
    let localeLanguage = 'zh-CN';
    if (language) {
        switch (language) {
            case 'cn':
                localeLanguage = 'zh-CN';
                break;
            case 'en':
                localeLanguage = 'en-US';
                break;
            case 'ja':
                localeLanguage = 'ja-JA';
                break;
            case 'fr':
                localeLanguage = 'fr-FR';
                break;
            case 'ru':
                localeLanguage = 'ru-RU';
                break;
            default:
                localeLanguage = 'zh-CN';
        }
    }
    localStorage.setItem('locale', localeLanguage);
}
// 登录跳转kvmJava下载的场景：
// 1.search的参数含有‘java’或‘jre’，即32行
// 2.https://IP/remoteconsole
// 3.https://IP/kvmvmm.asp
// 4.https://IP/login.html?redirect_type=1
if (search.indexOf('html5') < 0 &&
    (kvmMark.indexOf(pathname.split('/')[1]) > -1 || window.location.href.indexOf('login.html?redirect_type=1') > -1) ) {
        localStorage.setItem('kvmJava', '1');
}
if (kvmMark.indexOf(pathname.split('/')[1]) > -1) {
    sessionStorage.setItem('kvmConnect', '1');
    if (search) {
        sessionStorage.setItem('kvmSearch', search);
    }
} else if (window.location.href.indexOf('login.html?redirect_type=1') > -1) {
    sessionStorage.setItem('kvmConnect', '1');
    if (search) {
        sessionStorage.setItem('kvmSearch', search);
    }
}


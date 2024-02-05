import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { CommonModule, HashLocationStrategy, LocationStrategy } from '@angular/common';

import { FormsModule } from '@angular/forms';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { HttpClientModule, HttpClient } from '@angular/common/http';
import { TiMessageModule, TiLocale } from '@cloud/tiny3';
import { TranslateModule, TranslateLoader } from '@ngx-translate/core';
// 服务导入
import {
    HttpService,
    UserInfoService,
    TimeoutService,
    LanguageService,
    AlertMessageService,
    GlobalDataService,
    HelpRelationService,
    LoadingService,
    ErrortipService,
    CommonService,
    SystemLockService
} from './common-app/service';
import {
    KvmLoginGuard,
    AppGuard,
    FansGuard,
    FdmGuard,
    IbmaGuard,
    LicenseGuard,
    OnlineGuard,
    PartityGuard,
    InfoGuard,
    SpGuard,
    USBGuard
} from './common-app/guard';
import { httpInterceptorProviders } from './common-app/http-interceptors';
import { TranslateHttpLoader } from '@ngx-translate/http-loader';

// 国际化配置
export function createTranslateHttpLoader(http: HttpClient): TranslateHttpLoader {
    return new TranslateHttpLoader(http, 'assets/common-assets/i18n/', '.json');
}

@NgModule({
    declarations: [
        AppComponent
    ],
    imports: [
        CommonModule,
        BrowserModule,
        BrowserAnimationsModule,
        FormsModule,
        AppRoutingModule,
        HttpClientModule,
        TiMessageModule,
        TranslateModule.forRoot({
            loader: {
                provide: TranslateLoader,
                useFactory: createTranslateHttpLoader,
                deps: [HttpClient]
            }
        }),
    ],
    providers: [
        LanguageService,
        AlertMessageService,
        GlobalDataService,
        HelpRelationService,
        LoadingService,
        TimeoutService,
        UserInfoService,
        ErrortipService,
        HttpService,
        httpInterceptorProviders,
        CommonService,
        SystemLockService,
        KvmLoginGuard,
        AppGuard,
        FansGuard,
        FdmGuard,
        IbmaGuard,
        LicenseGuard,
        OnlineGuard,
        PartityGuard,
        InfoGuard,
        SpGuard,
        USBGuard,
        {
            provide: LocationStrategy,
            useClass: HashLocationStrategy
        }
    ],
    bootstrap: [AppComponent]
})
export class AppModule {
    constructor(

    ) {
        // tiny3配置语言
        TiLocale.setLocale(TiLocale.EN_US);
    }
}

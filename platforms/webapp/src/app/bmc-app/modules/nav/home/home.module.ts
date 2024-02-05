import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';

import { TiModalModule, TiMessageModule, TiTipModule, TiButtonModule, TiFormfieldModule } from '@cloud/tiny3';

import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './home.component';
import { HomeCollectModule } from 'src/app/common-app/components/home-collect/home-collect.module';
import { HomeLoginInforModule } from 'src/app/common-app/components/home-login-infor/home-login-infor.module';
import {
    HomeResourceComponent,
    HomeQuickAccessComponent,
    HomeVirtualControllerComponent,
    HomeVirtualModalComponent,
    HomeDetailComponent,
    HomeDetailSimpleComponent,
    HomeDetailBeautifulComponent,
    HomeDetailStandardComponent,
    HomeDefaultComponent,
    HomeSimpleComponent,
    HomeBeautifulComponent,
    HomeStandardComponent,
    HomeMonitorComponent
} from './component';
import { HomeService } from './services/home.service';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import _echarts from 'src/app/common-app/utils/custom-echarts';
import { NgxEchartsModule } from 'ngx-echarts';

@NgModule({
    declarations: [
        HomeComponent,
        HomeDetailComponent,
        HomeDetailBeautifulComponent,
        HomeDetailSimpleComponent,
        HomeDetailStandardComponent,
        HomeResourceComponent,
        HomeQuickAccessComponent,
        HomeVirtualControllerComponent,
        HomeVirtualModalComponent,
        HomeDefaultComponent,
        HomeSimpleComponent,
        HomeBeautifulComponent,
        HomeStandardComponent,
        HomeMonitorComponent
    ],
    providers: [HomeService],
    imports: [
        CommonModule,
        HomeRoutingModule,
        HomeCollectModule,
        HomeLoginInforModule,
        HttpClientModule,
        NgxEchartsModule.forRoot({ echarts: Promise.resolve(_echarts) }),
        TiModalModule,
        TiMessageModule,
        TiButtonModule,
        TiFormfieldModule,
        TiTipModule,
        PipeModule,
        TranslateModule,
    ]
})
export class HomeModule { }

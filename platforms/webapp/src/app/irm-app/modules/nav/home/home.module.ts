import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { NgxEchartsModule } from 'ngx-echarts';

import {
    TiModalModule,
    TiMessageModule,
    TiButtonModule,
    TiFormfieldModule,
    TiStepsModule,
    TiTipModule,
    TiTableModule,
    TiPaginationModule } from '@cloud/tiny3';

import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './home.component';
import { HomeCollectModule } from 'src/app/common-app/components/home-collect/home-collect.module';
import { HomeLoginInforModule } from 'src/app/common-app/components/home-login-infor/home-login-infor.module';
import {
    HomeIrmDetailComponent,
    HomeIrmQuickAccessComponent,
    HomeIrmResourceComponent,
    HomeIrmAssetComponent,
	HomeIrmCabinetComponent,
    HomeIrmQuickConfigComponent
} from './component';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { HomeService } from './services';

@NgModule({
    declarations: [
        HomeComponent,
        HomeIrmDetailComponent,
        HomeIrmQuickAccessComponent,
        HomeIrmAssetComponent,
        HomeIrmResourceComponent,
        HomeIrmCabinetComponent,
	    HomeIrmQuickConfigComponent
    ],
    providers: [HomeService],
    imports: [
        CommonModule,
        HomeRoutingModule,
        HomeCollectModule,
        HomeLoginInforModule,
        TiTipModule,
        TiTableModule,
        TiPaginationModule,
        HttpClientModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
        TiModalModule,
        TiMessageModule,
        TiButtonModule,
        TiFormfieldModule,
	    TiStepsModule,
        PipeModule,
        TranslateModule,
    ]
})
export class HomeModule { }

import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { NgxEchartsModule } from 'ngx-echarts';

import { TiModalModule, TiMessageModule, TiTipModule, TiButtonModule, TiFormfieldModule } from '@cloud/tiny3';

import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './home.component';
import {
    HomeResourceComponent,
    HomeQuickAccessComponent,
    HomeLoginInforComponent,
    HomeVirtualControllerComponent,
    HomeVirtualModalComponent,
    HomeDetailComponent,
    HomeDefaultComponent,
    HomeMonitorComponent
} from './component';
import { HomeService } from './services/home.service';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';


@NgModule({
    declarations: [
        HomeComponent,
        HomeDetailComponent,
        HomeResourceComponent,
        HomeQuickAccessComponent,
        HomeLoginInforComponent,
        HomeVirtualControllerComponent,
        HomeVirtualModalComponent,
        HomeDefaultComponent,
        HomeMonitorComponent
    ],
    entryComponents: [HomeVirtualModalComponent],
    providers: [HomeService],
    imports: [
        CommonModule,
        HomeRoutingModule,
        HttpClientModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
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

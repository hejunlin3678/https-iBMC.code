import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HttpClientModule, HttpClient } from '@angular/common/http';
import { TranslateModule, TranslateLoader } from '@ngx-translate/core';
import { TranslateHttpLoader } from '@ngx-translate/http-loader';
import { NgxEchartsModule } from 'ngx-echarts';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TiModalModule, TiMessageModule, TiTextModule, TiTipModule, TiButtonModule, TiValidationModule, TiFormfieldModule } from '@cloud/tiny3';
import { TpIconModule } from '@cloud/tinyplus3';

import { HomeRoutingModule } from './home-routing.module';
import { HomeComponent } from './home.component';
import {
    HomeEmmDetailComponent,
    HomeEmmQuickAccessComponent,
    HomeEmmResourceComponent,
    HomeEmmCollectComponent,
	HomeEmmLoginInforComponent,
    HomeEmmSystemMointorComponent
} from './component';
import { HomeService } from './services';
import { E9000FrontAndRearViewComponent } from './component/home-detail/component/e9000FrontAndRearView/e9000-frontAndRearView.component';
import { Tce8080FrontAndRearViewComponent } from './component/home-detail/component/tce8080FrontAndRearView/tce8080-frontAndRearView.component';
import { Tce8040FrontAndRearViewComponent } from './component/home-detail/component/tce8040FrontAndRearView/tce8040-frontAndRearView.component';
import { PowerStateImagePipe } from './component/home-detail/pipes/powerStateImage.pipe';
import { HealthImagePipe } from './component/home-detail/pipes/healthImage.pipe';

// 国际化配置
export function createTranslateHttpLoader(http: HttpClient) {
    return new TranslateHttpLoader(http, 'assets/i18n/', '.json');
}

@NgModule({
    declarations: [
        HomeComponent,
        HomeEmmDetailComponent,
        HomeEmmQuickAccessComponent,
        HomeEmmCollectComponent,
        HomeEmmResourceComponent,
        HomeEmmLoginInforComponent,
        HomeEmmSystemMointorComponent,
        E9000FrontAndRearViewComponent,
        Tce8080FrontAndRearViewComponent,
        Tce8040FrontAndRearViewComponent,
        PowerStateImagePipe,
        HealthImagePipe,

    ],
    providers: [HomeService],
    imports: [
        CommonModule,
        HomeRoutingModule,
        HttpClientModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
        FormsModule,
        ReactiveFormsModule,
        TiModalModule,
        TiMessageModule,
        TiTextModule,
        TiButtonModule,
        TiValidationModule,
        TiFormfieldModule,
        TiTipModule,
        TpIconModule,
        TranslateModule.forChild({
            loader: {
                provide: TranslateLoader,
                useFactory: createTranslateHttpLoader,
                deps: [HttpClient]
            }
        }),
    ]
})
export class HomeModule { }

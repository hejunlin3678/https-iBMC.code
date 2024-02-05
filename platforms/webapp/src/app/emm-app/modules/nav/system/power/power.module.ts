import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { PowerRoutingModule } from './power-routing.module';
import { PowerComponent } from './power.component';
import {
    TiTabModule,
    TiButtonModule,
    TiFormfieldModule,
    TiModalModule,
    TiRadioModule,
    TiCheckboxModule,
    TiSwitchModule,
    TiTipModule,
    TiButtongroupModule,
    TiValidationModule,
    TiDatetimeRangeModule,
    TiTextModule,
    TiOverflowModule,
    TiTableModule,
    TiProgressbarModule
} from '@cloud/tiny3';
import { HttpClient } from '@angular/common/http';
import { TranslateHttpLoader } from '@ngx-translate/http-loader';
import { TranslateModule, TranslateLoader } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { NgxEchartsModule } from 'ngx-echarts';
import { TpLabeleditorModule } from '@cloud/tinyplus3';
import { PowerStateComponent } from './component/power-state/power-state.component';
import { PowerHistoryEmmComponent } from './component/power-history-emm/power-history-emm.component';
import { PowerCappingEmmComponent } from './component/power-capping-emm/power-capping-emm.component';
import { ChassisPowerCappingComponent} from './component/power-capping-emm/component/chassis-power-capping/chassis-power-capping.component';
import { BladePowerCappingComponent } from './component/power-capping-emm/component/blade-power-capping/blade-power-capping.component';
import { PowerSleepComponent } from './component/power-state/component/power-sleep/power-sleep.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

export function createTranslateHttpLoader(http: HttpClient) {
    return new TranslateHttpLoader(http, 'src/assets/i18n/', '.json');
}

@NgModule({
    declarations: [
        PowerComponent,
        PowerStateComponent,
        PowerHistoryEmmComponent,
        PowerCappingEmmComponent,
        ChassisPowerCappingComponent,
        BladePowerCappingComponent,
        PowerSleepComponent
    ],
    imports: [
        CommonModule,
        PowerRoutingModule,
        TranslateModule.forChild({
            loader: {
                provide: TranslateLoader,
                useFactory: createTranslateHttpLoader,
                deps: [HttpClient]
            }
        }),
        FormsModule,
        ReactiveFormsModule,
        TiTabModule,
        TiButtonModule,
        TiFormfieldModule,
        TiModalModule,
        TiRadioModule,
        TiCheckboxModule,
        TiSwitchModule,
        TiTipModule,
        TiButtongroupModule,
        TiValidationModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
        TiDatetimeRangeModule,
        TpLabeleditorModule,
        TiTextModule,
        TiOverflowModule,
        CommonsModule,
        TiTableModule,
        TiProgressbarModule
    ]
})
export class PowerModule { }

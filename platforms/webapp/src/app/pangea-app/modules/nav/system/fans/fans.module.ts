import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { FansRoutingModule } from './fans-routing.module';
import { FansComponent } from './fans.component';
import { TranslateModule } from '@ngx-translate/core';
import { TiButtonModule,
    TiFormfieldModule,
    TiAlertModule, TiTextModule,
    TiModalModule,
    TiValidationModule,
    TiTipModule,
    TiSwitchModule
} from '@cloud/tiny3';
import { AirInletTemperatureComponent } from './component/air-inlet-temperature/air-inlet-temperature.component';
import { FanInfoComponent } from './component/fan-info/fan-info.component';
import { NgxEchartsModule } from 'ngx-echarts';
import { IntelSpeedAdjustComponent } from './component/intel-speed-adjust/intel-speed-adjust.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';


@NgModule({
    declarations: [FansComponent, AirInletTemperatureComponent, FanInfoComponent, IntelSpeedAdjustComponent],
    imports: [
        CommonModule,
        FansRoutingModule,
        TranslateModule,
        FormsModule,
        CommonsModule,
        ReactiveFormsModule,
        NgxEchartsModule.forRoot({
            echarts: () => import('echarts')
        }),
        TiButtonModule,
        TiFormfieldModule,
        TiAlertModule,
        TiTextModule,
        TiModalModule,
        TiTipModule,
        TiValidationModule,
        TiSwitchModule
    ],
    entryComponents: [IntelSpeedAdjustComponent]
})
export class FansModule { }

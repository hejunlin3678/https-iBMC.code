import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { FansRoutingModule } from './fans-routing.module';
import { FansComponent } from './fans.component';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiButtonModule,
    TiFormfieldModule,
    TiAlertModule, TiTextModule,
    TiModalModule,
    TiValidationModule,
    TiTipModule,
    TiTableModule,
    TiProgressbarModule
} from '@cloud/tiny3';
import { CoolingModeSettingComponent } from './component/cooling-mode-setting/cooling-mode-setting.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';


@NgModule({
    declarations: [FansComponent, CoolingModeSettingComponent],
    imports: [
        CommonModule,
        FansRoutingModule,
        TranslateModule,
        FormsModule,
        CommonsModule,
        ReactiveFormsModule,
        TiButtonModule,
        TiFormfieldModule,
        TiAlertModule,
        TiTextModule,
        TiModalModule,
        TiTipModule,
        TiValidationModule,
        TiTableModule,
        TiProgressbarModule,
        PipeModule
    ]
})
export class FansModule { }

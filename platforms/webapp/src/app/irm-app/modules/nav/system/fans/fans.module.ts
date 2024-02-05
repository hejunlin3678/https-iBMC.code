import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { FansRoutingModule } from './fans-routing.module';
import { FansComponent } from './fans.component';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiFormfieldModule,
    TiAlertModule, TiTextModule,
    TiTipModule,
    TiSwitchModule
} from '@cloud/tiny3';
import { FanInfoComponent } from './component/fan-info/fan-info.component';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';


@NgModule({
    declarations: [FansComponent, FanInfoComponent],
    imports: [
        CommonModule,
        FansRoutingModule,
        TranslateModule,
        FormsModule,
        CommonsModule,
        ReactiveFormsModule,
        TiFormfieldModule,
        TiAlertModule,
        TiTextModule,
        TiTipModule,
        TiSwitchModule
    ]
})
export class FansModule { }

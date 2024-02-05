import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TpcmRoutingModule } from './tpcm-routing.module';
import {
    TiFormfieldModule,
    TiModalModule,
    TiTableModule,
    TiOverflowModule,
    TiIconModule,
    TiButtonModule,
    TiTextModule,
    TiValidationModule,
    TiAlertModule,
    TiSwitchModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { TranslateModule } from '@ngx-translate/core';
import { TpcmComponent } from './tpcm.component';
@NgModule({
    declarations: [TpcmComponent],
    imports: [
        TpcmRoutingModule,
        CommonModule,
        TranslateModule,
        TiFormfieldModule,
        TiModalModule,
        TiTableModule,
        TiIconModule,
        FormsModule,
        ReactiveFormsModule,
        TiOverflowModule,
        CommonsModule,
        TiButtonModule,
        TiTextModule,
        TiValidationModule,
        TiAlertModule,
        TiSwitchModule
    ]
})
export class TpcmModule {}

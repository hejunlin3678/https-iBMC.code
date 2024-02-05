import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { RestSwiRoutingModule } from './rest-swi-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import {
    TiButtonModule,
    TiTableModule,
    TiOverflowModule,
    TiCollapseModule,
    TiTipModule,
    TiModalModule,
    TiMessageModule,
    TiAlertModule,
    TiCheckboxModule,
    TiIconModule
} from '@cloud/tiny3';
import { RestSwiComponent } from './rest-swi.component';
import { ConfigRecoveryComponent } from './component/config-recovery/config-recovery.component';
import { SwiDetailComponent } from './component/swi-detail/swi-detail.component';
import { PipeModule } from 'src/app/common-app/pipe/pipe.module';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

@NgModule({
    declarations: [
        RestSwiComponent,
        ConfigRecoveryComponent,
        SwiDetailComponent
    ],
    imports: [
        CommonModule,
        TiButtonModule,
        TiTableModule,
        TiOverflowModule,
        TiCollapseModule,
        TiTipModule,
        TiModalModule,
        TiMessageModule,
        TiAlertModule,
        TiCheckboxModule,
        TiIconModule,
        TranslateModule,
        PipeModule,
        FormsModule,
        ReactiveFormsModule,
        CommonsModule,
        RestSwiRoutingModule
    ]
})
export class RestSwiModule { }

import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { InfoComponent } from './info.component';
import { InfoRoutingModule } from './info-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

@NgModule({
    declarations: [
        InfoComponent
    ],
    imports: [
        CommonModule,
        InfoRoutingModule,
        TranslateModule,
        CommonsModule
    ]
})
export class InfoModule { }

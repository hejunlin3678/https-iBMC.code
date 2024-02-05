import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { InfoComponent } from './info.component';
import { InfoRoutingModule } from './info-routing.module';
import { TranslateModule } from '@ngx-translate/core';

@NgModule({
    declarations: [
        InfoComponent
    ],
    imports: [
        CommonModule,
        InfoRoutingModule,
        TranslateModule
    ]
})
export class InfoModule { }

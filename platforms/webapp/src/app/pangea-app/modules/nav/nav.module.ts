import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { NavRoutingModule } from './nav-routing.module';
import { TiAlertModule } from '@cloud/tiny3';
import { NavComponent } from './nav.component';
import { TranslateModule } from '@ngx-translate/core';
import { HeaderModule } from 'src/app/common-app/components/header/header.module';

import { CommonsModule } from 'src/app/common-app/components/commons.module';

@NgModule({
    declarations: [NavComponent],
    imports: [
        CommonModule,
        CommonsModule,
        HeaderModule,
        TiAlertModule,
        NavRoutingModule,
        TranslateModule,
    ],
    entryComponents: [],
    providers: []
})
export class NavModule { }

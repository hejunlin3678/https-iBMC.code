import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { BmcUserRoutingModule } from './bmc-user-routing.module';
import { TranslateModule } from '@ngx-translate/core';
import { UsersComponent } from './components/users.component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import {
    TiButtonModule,
    TiCheckboxModule,
    TiCheckgroupModule,
    TiFormfieldModule,
    TiIconModule,
    TiModalModule,
    TiSelectModule,
    TiTableModule,
    TiTextModule,
    TiValidationModule
} from '@cloud/tiny3';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { UserManagerComponent } from './components/user-manager/user-manager.component';
import { EmmSecondComponent } from './components/emm-second/emm-second.component';

@NgModule({
    declarations: [
        UsersComponent,
        UserManagerComponent,
        EmmSecondComponent
    ],
    imports: [
        CommonModule,
        CommonsModule,
        FormsModule,
        ReactiveFormsModule,
        TiTextModule,
        TiFormfieldModule,
        TiButtonModule,
        TiSelectModule,
        TiTableModule,
        TiCheckgroupModule,
        TiCheckboxModule,
        TiIconModule,
        TiModalModule,
        TiValidationModule,
        TranslateModule,
        BmcUserRoutingModule,
    ]
})
export class BmcUserModule { }

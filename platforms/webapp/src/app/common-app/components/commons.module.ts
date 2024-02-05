import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { SecondDropComponent } from './second-drop/second-drop.component';
import { SecondPasswordComponent } from 'src/app/common-app/components/second-password/second-password.component';
import { TiModalModule, TiButtonModule, TiIconModule, TiTextModule, TiAlertModule } from '@cloud/tiny3';
import { TranslateModule } from '@ngx-translate/core';

import { PartialLoadingComponent } from 'src/app/common-app/components/partial-loading/partial-loading.component';
import { PrivilPipe } from 'src/app/common-app/pipe/privil.pipe';
import { RolePipe } from 'src/app/common-app/pipe/role.pipe';
import { RulePipe } from 'src/app/common-app/pipe/rule.pipe';
import { LockAlertComponent } from './lock-alert/lock-alert.component';
import { CustHalfModalComponent } from './cust-half-modal/cust-half-modal.component';
import { TpHalfmodalModule } from '@cloud/tinyplus3';

import { RefireDirective } from '../directives/refire.directive';
import { LoadingComponent } from './loading/loading.component';

@NgModule({
    declarations: [
        SecondDropComponent,
        SecondPasswordComponent,
        PartialLoadingComponent,
        PrivilPipe,
        RolePipe,
        RulePipe,
        LockAlertComponent,
        CustHalfModalComponent,
        RefireDirective,
        LoadingComponent
    ],
    imports: [
        CommonModule,
        FormsModule,
        TiModalModule,
        TiButtonModule,
        TiIconModule,
        TiTextModule,
        TiAlertModule,
        TpHalfmodalModule,
        TranslateModule
    ],
    exports: [
        SecondDropComponent,
        SecondPasswordComponent,
        PartialLoadingComponent,
        PrivilPipe,
        RolePipe,
        RulePipe,
        LockAlertComponent,
        CustHalfModalComponent,
        RefireDirective,
        LoadingComponent
    ]
})
export class CommonsModule { }

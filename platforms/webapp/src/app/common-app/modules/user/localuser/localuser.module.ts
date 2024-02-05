import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import {
    TiValidationModule,
    TiButtonModule,
    TiTableModule,
    TiOverflowModule,
    TiFormfieldModule,
    TiSelectModule,
    TiSwitchModule,
    TiCheckboxModule,
    TiIconModule,
    TiTipModule,
    TiRadioModule,
    TiMessageModule,
    TiTabModule,
    TiModalModule,
    TiCheckgroupModule,
    TiUploadModule,
    TiAlertModule,
    TiSearchboxModule,
    TiTextModule,
    TiTextareaModule,
    TiCollapseModule
} from '@cloud/tiny3';
import { LocaluserRoutingModule } from './localuser-routing.module';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { CommonsModule } from 'src/app/common-app/components/commons.module';
import { TpHalfmodalModule, TpHelptipModule } from '@cloud/tinyplus3';
import { TranslateModule } from '@ngx-translate/core';
import { LocalUserService } from './localuser.service';
import { LocaluserComponent } from './localuser.component';
import { SshUploadComponent } from './component/ssh-upload/ssh-upload.component';
import { UserInfoTemplateComponent } from './component/user-info-template/user-info-template.component';
import { LoginInterfaceTipComponent } from './component/login-interface-tip/login-interface-tip.component';


@NgModule({
    declarations: [
        LocaluserComponent,
        SshUploadComponent,
        UserInfoTemplateComponent,
        LoginInterfaceTipComponent,
    ],
    imports: [
        CommonModule,
        LocaluserRoutingModule,
        FormsModule,
        ReactiveFormsModule,
        CommonsModule,
        TiValidationModule,
        TiButtonModule,
        TiTableModule,
        TiOverflowModule,
        TpHalfmodalModule,
        TpHelptipModule,
        TiFormfieldModule,
        TiSelectModule,
        TiSwitchModule,
        TiCheckboxModule,
        TiIconModule,
        TiTipModule,
        TiSwitchModule,
        TiRadioModule,
        TiMessageModule,
        TiValidationModule,
        TiTabModule,
        TiModalModule,
        TiCheckgroupModule,
        TiUploadModule,
        TiAlertModule,
        TiSearchboxModule,
        TiTextModule,
        TiTextareaModule,
        TiCollapseModule,
        TranslateModule
    ],
    providers: [LocalUserService]
})
export class LocaluserModule { }

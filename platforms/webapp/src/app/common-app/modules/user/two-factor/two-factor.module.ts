import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

import { TwoFactorRoutingModule } from './two-factor-routing.module';
import { ClientCertificateComponent } from './component/client-certificate/client-certificate.component';
import { RootCertificateComponent } from './component/root-certificate/root-certificate.component';
import { TranslateModule } from '@ngx-translate/core';
import { TwoFactorComponent } from './two-factor.component';
import { FormsModule } from '@angular/forms';
import {
    TiTabModule, TiFormfieldModule, TiSwitchModule, TiButtonModule, TiTableModule, TiMessageModule,
    TiUploadModule, TiTipModule, TiOverflowModule
} from '@cloud/tiny3';

@NgModule({
    declarations: [
        TwoFactorComponent,
        ClientCertificateComponent,
        RootCertificateComponent
    ],
    imports: [
        CommonModule,
        CommonsModule,
        TwoFactorRoutingModule,
        TranslateModule,
        FormsModule,
        TiTabModule,
        TiFormfieldModule,
        TiSwitchModule,
        TiButtonModule,
        TiTableModule,
        TiMessageModule,
        TiUploadModule,
        TiTipModule,
        TiOverflowModule
    ]
})
export class TwoFactorModule { }

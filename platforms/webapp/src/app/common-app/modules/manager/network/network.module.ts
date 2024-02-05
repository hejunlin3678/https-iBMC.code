import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { NetworkRoutingModule } from './network-routing.module';
import {
    TiLeftmenuModule,
    TiSelectModule,
    TiFormfieldModule,
    TiUploadModule,
    TiModalModule,
    TiButtonModule,
    TiTableModule,
    TiSwitchModule,
    TiAlertModule,
    TiProgressbarModule,
    TiValidationModule,
    TiRadioModule, TiIpModule,
    TiCheckgroupModule,
    TiCheckboxModule,
    TiOverflowModule,
    TiTipModule,
    TiTabModule,
    TiTextModule,
    TiDateModule,
    TiPaginationModule
} from '@cloud/tiny3';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { TpLabeleditorModule, TpIconModule } from '@cloud/tinyplus3';
import { NetworkComponent } from './network.component';
import {
    NetworkDnsComponent,
    NetworkHostComponent,
    NetworkLldpComponent,
    NetworkPortComponent,
    NetworkProtocolComponent,
    NetworkVlanComponent
} from './component';
import { CommonsModule } from 'src/app/common-app/components/commons.module';


@NgModule({
    declarations: [
        NetworkComponent,
        NetworkDnsComponent,
        NetworkHostComponent,
        NetworkLldpComponent,
        NetworkPortComponent,
        NetworkProtocolComponent,
        NetworkVlanComponent
    ],
    imports: [
        CommonModule,
        NetworkRoutingModule,
        TiLeftmenuModule,
        HttpClientModule,
        TranslateModule,
        TiSelectModule,
        TiFormfieldModule,
        TiUploadModule,
        TiButtonModule,
        TiModalModule,
        TiTableModule,
        FormsModule,
        ReactiveFormsModule,
        TiSwitchModule,
        TiAlertModule,
        TiProgressbarModule,
        TpLabeleditorModule,
        TpIconModule,
        TiValidationModule,
        TiRadioModule,
        TiIpModule,
        TiCheckgroupModule,
        TiCheckboxModule,
        TiTipModule,
        TiOverflowModule,
        TiTabModule,
        TiTextModule,
        TiDateModule,
        TiPaginationModule,
        CommonsModule
    ]
})
export class NetworkModule { }

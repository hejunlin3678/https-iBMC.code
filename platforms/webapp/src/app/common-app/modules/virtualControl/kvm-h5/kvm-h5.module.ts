import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { KvmH5RoutingModule } from './kvm-h5-routing.module';
import { KvmH5Component } from './kvm-h5.component';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';
import { FormsModule } from '@angular/forms';
import { TiMessageModule, TiSliderModule, TiButtonModule, TiRadioModule, TiUploadModule } from '@cloud/tiny3';


@NgModule({
    declarations: [KvmH5Component],
    imports: [
        TiMessageModule,
        TiSliderModule,
        TiButtonModule,
        TiRadioModule,
        TiUploadModule,
        CommonModule,
        KvmH5RoutingModule,
        FormsModule,
        HttpClientModule,
        TranslateModule
    ]
})
export class KvmH5Module {}

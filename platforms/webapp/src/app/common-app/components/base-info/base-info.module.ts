import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { TiMessageModule } from '@cloud/tiny3';
import { BaseInfoComponent } from './base-info.component';

import { DropPowerComponent } from './drop-power/drop-power.component';
import { DropUidComponent } from './drop-uid/drop-uid.component';

import { CommonsModule } from '../commons.module';
import { DropLanguageModule } from './drop-language/drop-language.module';
import { LoginInfoComponent } from './login-info/login-info.component';

// 服务
import { HeaderService } from '../header/header.service';
import { TranslateModule } from '@ngx-translate/core';
import { AlarmComponent } from './alarm/alarm.component';
import { BaseInfoService } from './base-info.service';

// 国际化配置

@NgModule({
    declarations: [BaseInfoComponent, DropPowerComponent, DropUidComponent, LoginInfoComponent, AlarmComponent],
    imports: [
        CommonModule,
        CommonsModule,
        TiMessageModule,
        TranslateModule,
        DropLanguageModule
    ],
    providers: [HeaderService, BaseInfoService],
    exports: [BaseInfoComponent]
})
export class BaseInfoModule { }

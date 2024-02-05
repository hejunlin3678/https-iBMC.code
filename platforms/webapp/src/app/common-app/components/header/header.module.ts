import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { RouterModule } from '@angular/router';

// 自定义模块引用
import { BaseInfoModule } from '../base-info/base-info.module';
// tiny3的 tip模块
import { TiTipModule } from '@cloud/tiny3';

// 自定义组件引用
import { HeaderComponent } from './header/header.component';
import { FooterComponent } from './footer/footer.component';
import { DropDownComponent } from './drop-down/drop-down.component';

import { HeaderService } from './header.service';
import { TranslateModule } from '@ngx-translate/core';


// 国际化配置

@NgModule({
  declarations: [HeaderComponent, FooterComponent, DropDownComponent],
  imports: [
    CommonModule,
    TranslateModule,
    TiTipModule,
    BaseInfoModule,
    RouterModule
  ],
  providers: [HeaderService],
  exports: [HeaderComponent, FooterComponent]
})
export class HeaderModule { }

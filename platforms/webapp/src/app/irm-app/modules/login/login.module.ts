import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';

import { LoginRoutingModule } from './login-routing.module';
import { LoginComponent } from './login.component';

import { TiSelectModule, TiTipModule, TiIconModule, TiValidationModule } from '@cloud/tiny3';
import { HttpClientModule } from '@angular/common/http';
import { TranslateModule } from '@ngx-translate/core';

// 自定义模块导入
import { DropLanguageModule } from 'src/app/common-app/components/base-info/drop-language/drop-language.module';

import { CommonService } from 'src/app/common-app/service/common.service';


@NgModule({
	declarations: [LoginComponent],
	imports: [
		CommonModule,
		FormsModule,
		ReactiveFormsModule,
		DropLanguageModule,
		HttpClientModule,
		TiValidationModule,
		TranslateModule,
		TiSelectModule,
		TiTipModule,
		TiIconModule,
		LoginRoutingModule
	],
	providers: [CommonService]
})
export class LoginModule { }

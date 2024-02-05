import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { CommonsModule } from 'src/app/common-app/components/commons.module';

import { DropLanguageComponent } from './drop-language.component';

@NgModule({
    declarations: [DropLanguageComponent],
    imports: [
        CommonModule,
        CommonsModule
    ],
    exports: [DropLanguageComponent]
})
export class DropLanguageModule { }

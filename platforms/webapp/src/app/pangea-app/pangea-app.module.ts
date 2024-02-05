import { NgModule, APP_INITIALIZER, Injector } from '@angular/core';
import { LOCATION_INITIALIZED } from '@angular/common';

import { PangeaAppRoutingModule } from './pangea-app-routing.module';
import { PangeaAppComponent } from './pangea-app.component';
import { LanguageService } from '../common-app/service';
import { ArcticNavMenuService, AtlanticNavMenuService, AtlanticSMMNavMenuService, PacificNavMenuService } from './services';
import { TranslateService } from '@ngx-translate/core';
import { PangeaGuard } from './guard/pangea-guard.service';


export function languageInitFactory(translate: TranslateService, injector: Injector, languageService: LanguageService) {
    return () => new Promise<any>((resolve: any) => {
        const languageLocationInit = injector.get(LOCATION_INITIALIZED, Promise.resolve(null));
        languageLocationInit.then(() => {
            const locale = localStorage.getItem('locale') || languageService.getTargetLanguage();
            translate.setDefaultLang(locale);
            translate.use(locale).subscribe(() => {
            }, () => {
            }, () => {
                resolve(null);
            });
        });
    });
}

@NgModule({
    declarations: [PangeaAppComponent],
    imports: [
        PangeaAppRoutingModule
    ],
    providers: [
        ArcticNavMenuService,
        AtlanticNavMenuService,
        AtlanticSMMNavMenuService,
        PacificNavMenuService,
        PangeaGuard,
        {
            provide: APP_INITIALIZER,
            useFactory: languageInitFactory,
            deps: [TranslateService, Injector, LanguageService],
            multi: true
        }
    ]
})
export class PangeaAppModule { }

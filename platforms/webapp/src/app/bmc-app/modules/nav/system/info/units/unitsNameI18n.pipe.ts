import { Pipe, PipeTransform } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';
import { CardUrlArray } from './models/cardUrlArray';

@Pipe({
    name: 'unitsNameI18n'
})
export class UnitsNameI18nPipe implements PipeTransform {
    constructor(
        private i18n: TranslateService,
    ) { }
    transform(cardUrlArray: CardUrlArray, args?: any): string {
        let name = '';
        switch (cardUrlArray.getName) {
            case 'BasicComputingUnit':
                name = this.i18n.instant('COMPONENTS_BASIC_COMPUTING');
                break;
            case 'StorageExtensionUnit':
                name = this.i18n.instant('COMPONENTS_STORAGE_EXTENSION');
                break;
            case 'ExtensionUnit':
                name = this.i18n.instant('COMPONENTS_SYSTEM_EXTENSION');
                break;
            case 'IOExtensionUnit':
                name = this.i18n.instant('COMPONENTS_IO_EXTENSION');
                break;
            case 'CoolingUnit':
                name = this.i18n.instant('COMPONENTS_HEAT_DISSIPATION');
                break;
            default:
                break;
        }
        if (cardUrlArray.getMaxCount > 0) {
            name = name + ' (' + cardUrlArray.getTotalCount + '/' + cardUrlArray.getMaxCount + ')';
        }
        return name;
    }
}

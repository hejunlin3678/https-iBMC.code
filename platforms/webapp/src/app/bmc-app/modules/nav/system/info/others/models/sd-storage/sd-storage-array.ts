import { Card } from '../card';
import { SDStorage } from './sd-storage';

export class SDStorageArray extends Card {

    private sDStorages: SDStorage[];

    constructor() {
        super();
        this.title = [
            {
                title: 'COMMON_MANUFACTURER'
            },
            {
                title: 'COMMON_FIXED_VERSION'
            }
        ];
        this.sDStorages = [];
    }

    get getSDStorages(): SDStorage[] {
        return this.sDStorages;
    }

    addSDStorage(sDStorage: SDStorage) {
        this.sDStorages.push(sDStorage);
    }

}

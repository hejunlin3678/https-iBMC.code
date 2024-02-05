
import { IOptions } from 'src/app/common-app/models';
import { computeCapacity } from '../../utils/storage.utils';

export class AvailableCapacity {
    private label: string;
    public checked: boolean = false;
    private value: number;
    private unit: IOptions;
    private batyValue: number;

    // capacity���������MB��λ
    constructor(capacity: number) {
        const capacityObj = computeCapacity(capacity * 1024 * 1024);
        this.label = `${capacityObj.capacity}  ${capacityObj.unit.label}`;
        this.value = capacityObj.capacity;
        this.unit = capacityObj.unit;
        this.batyValue = capacity * 1024 * 1024;
    }

    get getValue() {
        return this.value;
    }

    get getLabel(): string {
        return this.label;
    }
    get getUnit() {
        return this.unit;
    }

    get getBatyValue() {
        return this.batyValue;
    }
}

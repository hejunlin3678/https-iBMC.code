import { BasicComputationComponent } from './components/basic-computation/basic-computation.component';


const dynamicComponent = {
    BasicComputingUnit: BasicComputationComponent,
    StorageExtensionUnit: BasicComputationComponent,
    IOExtensionUnit: BasicComputationComponent,
    CoolingUnit: BasicComputationComponent,
    ExtensionUnit: BasicComputationComponent
};

const importComponents = [
    BasicComputationComponent,
];

export { dynamicComponent, importComponents };

import { IoBoardComponent, EcuBoardComponent } from './components';

const dynamicComponent = {
    IOBoard: IoBoardComponent,
    EcuBoard: EcuBoardComponent
};

const importComponents = [IoBoardComponent, EcuBoardComponent];

export { dynamicComponent, importComponents };

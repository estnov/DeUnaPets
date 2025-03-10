import { HttpClientModule } from '@angular/common/http';
import { Component } from '@angular/core';
import { NzAvatarModule } from 'ng-zorro-antd/avatar';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzIconModule } from 'ng-zorro-antd/icon';

@Component({
  selector: 'app-home-page2',
  imports: [
    NzButtonModule,
    HttpClientModule,
    NzIconModule,
    NzAvatarModule],
  templateUrl: './home-page2.component.html',
  styleUrl: './home-page2.component.scss'
})
export class HomePage2Component {

}

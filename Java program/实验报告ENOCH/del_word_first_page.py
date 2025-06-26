# ! pip install pywin32

# 脚本用于删除当前目录下所有Word文档的第一页
# 以清除敏感信息

import os
import win32com.client as win32
import sys

def delete_first_page_of_word_docs():
    current_dir = os.getcwd()
    print(f"正在扫描文件夹: {current_dir}")
    word_app = None
    try:
        word_app = win32.Dispatch("Word.Application")
        word_app.Visible = False

        for filename in os.listdir(current_dir):
            if filename.lower().endswith((".docx", ".doc")) and not filename.startswith('~'):
                file_path = os.path.join(current_dir, filename)
                print(f"正在处理: {filename}...")

                doc = None
                try:
                    doc = word_app.Documents.Open(file_path)
                    word_app.Selection.GoTo(What=1, Which=1, Count=1) # wdGoToPage, wdGoToAbsolute
                    doc.Bookmarks("\\Page").Select()
                    word_app.Selection.Delete()
                    doc.Close(SaveChanges=True)
                    print(f"  -> 已成功删除 {filename} 的第一页。")
                except Exception as e:
                    print(f"  -> 处理文件 {filename} 时发生错误: {e}")
                    if doc:
                        doc.Close(SaveChanges=False)
    
    except Exception as e:
        print(f"\n错误: {e}")

    finally:
        if word_app:
            word_app.Quit()
        print("\n处理完成。")


if __name__ == '__main__':
    print("="*50)
    print("警告：此脚本将删除当前文件夹下所有Word文档的第一页")
    print("="*50)
    
    user_confirmation = input("您确定要继续吗？ (输入 'y' 继续): ")
    
    if user_confirmation.lower() == 'y':
        delete_first_page_of_word_docs()
    else:
        print("操作已取消")

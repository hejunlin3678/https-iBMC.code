use bytes::{Buf, BufMut, Bytes, BytesMut};
use std::mem;

pub(super) struct Buffer {
    buf1: Bytes,
    buf2: Bytes,
}

impl Buffer {
    pub fn new() -> Self {
        Self {
            buf1: Bytes::new(),
            buf2: Bytes::new(),
        }
    }

    pub fn set(&mut self, data: Bytes) {
        self.buf1 = data;
        self.buf2.clear();
    }

    pub fn put(&mut self, data: Bytes) {
        if data.is_empty() {
            return;
        }

        if self.buf1.is_empty() {
            self.buf1 = data;
        } else if self.buf2.is_empty() {
            self.buf2 = data;
        } else {
            self.flush();
            self.buf2 = data;
        }
    }

    pub fn is_empty(&self) -> bool {
        return self.buf1.is_empty();
    }

    pub fn len(&self) -> usize {
        return self.buf1.len() + self.buf2.len();
    }

    pub fn skip(&mut self, len: usize) {
        debug_assert!((self.buf1.len() + self.buf2.len()) >= len);

        if self.buf1.len() > len {
            self.buf1.advance(len);
        } else {
            let buf1 = mem::replace(&mut self.buf1, mem::take(&mut self.buf2));
            self.buf1.advance(len - buf1.len());
        }
    }

    pub fn bytes(&mut self) -> &Bytes {
        self.flush();
        &self.buf1
    }

    pub fn read_until_boundary(&mut self, boundary: &[u8]) -> Option<(Bytes, bool)> {
        if self.buf1.len() >= boundary.len() {
            // buf1 的长度大于 boundary 的长度
            return match find_bytes(&self.buf1, boundary) {
                Some(i) => Some((self.buf1.split_to(i), true)),
                None => {
                    // 没有找到，考虑到 buf1 末尾可能存在部分 boundary 的可能，
                    // 留下 boundary.len() - 1 的内容确保不出现 boundary 截断问题
                    let bytes = self.buf1.split_to(self.buf1.len() - boundary.len() + 1);
                    Some((bytes, false))
                }
            };
        }

        let len = self.len();
        if len < boundary.len() {
            return None;
        }

        match self.find_boundary_in_buf1(boundary) {
            Some(i) => {
                // 找到 boundary，将 buf1 中 boundary 之前的内容返回
                Some((self.buf1.split_to(self.buf1.len() - i), true))
            }
            None => {
                // 没有找到，那只要在 buf1 + buf2 末尾留下 boundary.len() - 1 的内容，其他的返回
                let to_skip = len - boundary.len() + 1;
                let bytes = if to_skip < self.buf1.len() {
                    self.buf1.split_to(to_skip)
                } else {
                    // 这里只返回 buf1 的内容，可以节省一次 buf1 与 buf2 前半部的合并
                    mem::replace(&mut self.buf1, mem::take(&mut self.buf2))
                };
                Some((bytes, false))
            }
        }
    }

    fn find_boundary_in_buf1(&self, boundary: &[u8]) -> Option<usize> {
        let len = self.buf1.len();
        for i in (1..=len).rev() {
            let (needle1, needle2) = boundary.split_at(i);
            if &self.buf1[(len - i)..] == needle1 && self.buf2.starts_with(needle2) {
                return Some(i);
            }
        }

        None
    }

    #[inline]
    fn flush(&mut self) {
        if !self.buf2.is_empty() {
            self.buf1 = join_bytes(mem::take(&mut self.buf1), mem::take(&mut self.buf2));
        }
    }
}

fn join_bytes(buf1: Bytes, buf2: Bytes) -> Bytes {
    if buf1.is_empty() {
        buf2
    } else if buf2.is_empty() {
        buf1
    } else {
        let mut buf = BytesMut::with_capacity(buf1.len() + buf2.len());
        buf.put(buf1);
        buf.put(buf2);
        buf.freeze()
    }
}

pub fn find_bytes(buf: &[u8], needle: &[u8]) -> Option<usize> {
    memchr::memmem::find(buf, needle)
}

pub fn starts_with_between(buf1: &[u8], buf2: &[u8], needle: &[u8]) -> bool {
    let skip = buf1.len().min(needle.len());

    let (needle1, needle2) = needle.split_at(skip);
    &buf1[..skip] == needle1 && buf2.starts_with(needle2)
}

#[cfg(test)]
mod tests {
    use super::{join_bytes, Buffer};
    use bytes::Bytes;

    #[test]
    fn buffer_read_until_boundary() {
        let boundary = b"--AAAAAAAAAAAAAAAAAAA";
        let mut buf = Buffer::new();
        buf.put(Bytes::from_static(b"12345--AAAAAAAAAAAAAAAAAAA"));
        let result = buf.read_until_boundary(boundary);
        assert!(result.is_some());
        let (data, is_match) = result.unwrap();
        assert!(is_match);
        assert_eq!(&data[..], b"12345");
        assert_eq!(&buf.buf1[..], boundary);
    }

    #[test]
    fn buffer_read_until_boundary_1() {
        let boundary = b"--AAAAAAAAAAAAAAAAAAA";
        for i in 0..boundary.len() {
            let mut buf = Buffer::new();
            buf.put(join_bytes(
                Bytes::from_static(b"12345"),
                Bytes::copy_from_slice(&boundary[..i]),
            ));
            buf.put(Bytes::copy_from_slice(&boundary[i..]));
            let result = buf.read_until_boundary(boundary);
            assert!(result.is_some());
            let (mut data, is_match) = result.unwrap();
            if !is_match {
                let result1 = buf.read_until_boundary(boundary);
                assert!(result1.is_some());
                let (data2, is_match2) = result1.unwrap();
                assert!(is_match2);
                data = join_bytes(data, data2);
            }

            assert_eq!(&data[..], b"12345");
            assert_eq!(&buf.bytes()[..], boundary);
        }
    }

    #[test]
    fn buffer_read_until_boundary_2() {
        let boundary = b"--AAAAAAAAAAAAAAAAAAA";
        let mut buf = Buffer::new();
        buf.put(Bytes::from_static(b"12345--AAAAAAAAAAA"));
        buf.put(Bytes::from_static(b"Aaaaaaaaaaa"));
        let result = buf.read_until_boundary(boundary);
        assert!(result.is_some());

        // 不匹配，至少可以确定前面 `总长度 - boundary长度 + 1` 的内容不是 boundary 的一部分
        let (data, is_match) = result.unwrap();
        assert!(!is_match);
        assert_eq!(&data[..], b"12345--AA");
        assert_eq!(buf.len(), boundary.len() - 1);

        // 添加 boundary，再次匹配
        buf.put(Bytes::copy_from_slice(boundary));
        let result = buf.read_until_boundary(boundary);
        let (data, is_match) = result.unwrap();
        assert!(!is_match);
        assert_eq!(&data[..], b"AAAAAAAAAAaaaaaaaaaa");
        assert_eq!(&buf.buf1[..], boundary);
    }
}
